<template>
  <div class="page-map" style="display:flex; gap:16px; padding:16px;">
    <div style="width:320px">
      <el-card>
        <div style="display:flex;flex-direction:column;gap:10px">
          <el-select v-model="shipId" placeholder="选择船舶ID">
            <el-option v-for="s in ships" :key="s" :label="s" :value="s" />
          </el-select>
          <el-select v-model="basemap" placeholder="底图" clearable>
            <el-option v-for="(label, key) in basemapOptions" :key="key" :label="label" :value="key" />
          </el-select>
          <el-date-picker v-model="startTime" type="datetime" placeholder="开始时间" style="width:100%" />
          <el-date-picker v-model="endTime" type="datetime" placeholder="结束时间" style="width:100%" />
          <el-button type="primary" @click="fetchTracks">查询轨迹</el-button>
        </div>
      </el-card>
    </div>
    <div style="flex:1; height:720px; position:relative">
        <div id="map" style="width:100%; height:100%;"></div>
        <div style="position:absolute; right:18px; bottom:18px; z-index:999">
          <el-button size="small" @click="playPause">{{ playing ? '暂停' : '播放' }}</el-button>
          <el-button size="small" @click="resetPlay">重置</el-button>
        </div>
    </div>
  </div>
</template>

<script setup>
import { onMounted, ref, watch } from 'vue'
import 'leaflet/dist/leaflet.css'
import L from 'leaflet'
import { getShips, getVesselTracks } from '../api'
import dayjs from 'dayjs'

const ships = ref([])
const shipId = ref(null)
const startTime = ref(null)
const endTime = ref(null)
let map = null
let _layer = null
let tileLayerRef = null
  const tileErrors = {}
  const TILE_ERROR_THRESHOLD = 2

  function probeTileUrl(url) {
    // try loading a sample tile image (returns Promise<boolean>)
    return new Promise((resolve) => {
      try {
        const img = new Image()
        img.crossOrigin = 'anonymous'
        img.onload = () => resolve(true)
        img.onerror = () => resolve(false)
        // add cache-buster
        img.src = url + (url.includes('?') ? '&' : '?') + 'v=' + Date.now()
        // timeout fallback
        setTimeout(() => resolve(false), 4000)
      } catch (e) { resolve(false) }
    })
  }
let playMarker = null
let playing = false
let playIndex = 0
let playTimer = null
const basemap = ref('carto')

const basemapOptions = {
  carto: 'Carto Light',
  osm: 'OpenStreetMap',
  stamen: 'Stamen Toner Lite'
}

onMounted(async () => {
  try {
    const r = await getShips()
    ships.value = (r.data && r.data.ships) || []
  } catch (e) { console.error(e) }

  map = L.map('map').setView([30.0, 120.0], 6)
  // add initial tile layer via helper
  setTileLayer(basemap.value)

  // watch for basemap changes and switch tile layer cleanly
  watch(basemap, (newVal) => {
    if (!newVal) return
    try { setTileLayer(newVal) } catch (e) { console.error('切换底图失败', e) }
  })
})

async function fetchTracks() {
  if (!shipId.value) return
  const params = { ship_id: shipId.value }
  if (startTime.value) {
    params.start_time = dayjs(startTime.value).format('YYYY-MM-DD HH:mm:ss')
  }
  if (endTime.value) {
    params.end_time = dayjs(endTime.value).format('YYYY-MM-DD HH:mm:ss')
  }
  try {
    const r = await getVesselTracks(params)
    const data = (r.data && r.data.data) || []
    plotTracks(data)
  } catch (e) { console.error(e) }
}

async function plotTracks(data) {
  if (_layer) { map.removeLayer(_layer); _layer = null }
  if (!data || data.length === 0) return
  // normalize field names from backend (support multiple possible keys)
  function getLat(d) {
    return d.latitude ?? d.Lat ?? d.Latitude ?? d.lat ?? d.Lat_u ?? d.Lat_u ?? d.Latitude_u ?? d.Lat_u
  }
  function getLon(d) {
    return d.longitude ?? d.Long_u ?? d.Longitude ?? d.lon ?? d.Long ?? d.Longitude_u ?? d.Long_u
  }
  function getDatetime(d) {
    return d.datetime ?? d.DateTime_u ?? d.datetime_u ?? d.time ?? d.DateTime ?? null
  }
  function getWave(d) {
    return d.wave_height ?? d.WaveHeight ?? d.SurgeHeight ?? d.Wave_Height ?? d.waveheight ?? null
  }
  function getTemp(d) {
    return d.temperature ?? d.SeaTemp ?? d.Sea_Temp ?? d.sea_temp ?? null
  }

  const latlngs = data.map(d => [getLat(d), getLon(d)])
  _layer = L.layerGroup()

  // draw a light wide polyline as background and a thinner blue polyline on top for nicer look
  const polyBg = L.polyline(latlngs, { color: '#d7eaff', weight: 10, opacity: 0.95, smoothFactor: 1 })
  const poly = L.polyline(latlngs, { color: '#1f73ff', weight: 4, opacity: 1, smoothFactor: 1 })
  polyBg.addTo(_layer)
  poly.addTo(_layer)

  // custom start/end icons (HTML markers) for clear visualization
  const startIcon = L.divIcon({ className: 'start-marker', html: '<div class="start-dot"></div>', iconSize: [18,18], iconAnchor: [9,9] })
  const endIcon = L.divIcon({ className: 'end-marker', html: '<div class="end-dot"></div>', iconSize: [18,18], iconAnchor: [9,9] })

  data.forEach((d, idx) => {
    const lat = getLat(d), lon = getLon(d)
    if (lat == null || lon == null) return // skip invalid points
    // styled circle marker for each track point
    const m = L.circleMarker([lat, lon], {
      radius: 6,
      fillColor: '#ffffff',
      color: '#1f73ff',
      weight: 2,
      fillOpacity: 1
    })

    // build detailed popup content using database field names (formatted in Chinese)
    const DataID = d.DataID ?? d.dataid ?? d.id ?? null
    const ShipID = d.ShipID ?? d.ship_id ?? d.ShipId ?? d.shipId ?? null
    const DateTime = getDatetime(d)
    const WaveHeight = d.WaveHeight ?? d.wave_height ?? d.Wave_Height ?? d.SurgeHeight ?? null
    const SeaTemp = d.SeaTemp ?? d.sea_temp ?? d.Temperature ?? getTemp(d) ?? null
    const SurgeDirection = d.SurgeDirection ?? d.surge_direction ?? d.Surge_Direction ?? null
    const WavePeriod = d.WavePeriod ?? d.wave_period ?? d.Wave_Period ?? null
    const SurgeHeight = d.SurgeHeight ?? d.surge_height ?? null

    const rows = []
    if (DataID !== null && DataID !== undefined) rows.push(`<strong>编号:</strong> ${DataID}`)
    if (ShipID) rows.push(`<strong>船舶ID:</strong> ${ShipID}`)
    if (DateTime) rows.push(`<strong>时间:</strong> ${DateTime}`)
    rows.push(`<strong>经度:</strong> ${lon}`)
    rows.push(`<strong>纬度:</strong> ${lat}`)
    if (WaveHeight !== null && WaveHeight !== undefined) rows.push(`<strong>浪高:</strong> ${WaveHeight}`)
    if (SeaTemp !== null && SeaTemp !== undefined) rows.push(`<strong>海温:</strong> ${SeaTemp}`)
    if (SurgeDirection) rows.push(`<strong>涌流方向:</strong> ${SurgeDirection}`)
    if (WavePeriod !== null && WavePeriod !== undefined) rows.push(`<strong>波周期:</strong> ${WavePeriod}`)
    if (SurgeHeight !== null && SurgeHeight !== undefined) rows.push(`<strong>涌流高度:</strong> ${SurgeHeight}`)

    const popupHtml = `<div style=\"min-width:220px;line-height:1.5;font-size:13px\">${rows.join('<br/>')}</div>`

    m.bindPopup(popupHtml)
    // open popup on hover for quick inspection
    m.on('mouseover', () => m.openPopup())
    m.on('mouseout', () => m.closePopup())
    m.addTo(_layer)

    // add start/end markers
    if (idx === 0) {
      L.marker([lat, lon], { icon: startIcon }).addTo(_layer)
    }
    if (idx === data.length - 1) {
      L.marker([lat, lon], { icon: endIcon }).addTo(_layer)
    }
  })

  _layer.addTo(map)
  try { map.fitBounds(poly.getBounds(), { padding: [40,40] }) } catch (e) { console.warn(e) }

  // add arrow decorators if plugin available
  try {
    // dynamic import so app doesn't fail if dependency not installed
    const mod = await import('leaflet-polylinedecorator')
    if (mod && typeof L.polylineDecorator === 'function') {
      // pattern: repeated small arrowheads along the line
      const decorator = L.polylineDecorator(poly, {
        patterns: [
          { offset: 12, repeat: 24, symbol: L.Symbol.arrowHead({ pixelSize: 8, polygon: false, pathOptions: { stroke: true, color: '#1f73ff', weight:1 } }) }
        ]
      })
      decorator.addTo(_layer)
    }
  } catch (e) {
    // plugin not available or failed to load — ignore gracefully
    // console.warn('polyline decorator not available', e)
  }

  // prepare playback
  if (playMarker) { map.removeLayer(playMarker); playMarker = null }
  if (playTimer) { clearInterval(playTimer); playTimer = null }
  playIndex = 0
  playing = false
  const first = latlngs[0]
  playMarker = L.circleMarker(first, { radius: 8, color: '#ff4d4f', fillColor:'#fff', weight:2 }).addTo(map)
  // store points for play
  plotTracks._points = latlngs
}

async function setTileLayer(key) {
  const defs = {
    carto: {
      url: 'https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}.png',
      opts: { attribution: '&copy; CartoDB & OpenStreetMap', maxZoom: 19, subdomains: 'abcd' }
    },
    osm: {
      url: 'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
      opts: { attribution: '&copy; OpenStreetMap contributors', maxZoom: 19, subdomains: 'abc' }
    },
    stamen: {
      url: 'https://stamen-tiles.a.ssl.fastly.net/toner-lite/{z}/{x}/{y}.png',
      opts: { attribution: 'Map tiles by Stamen Design, CC BY 3.0 &mdash; Map data &copy; OpenStreetMap contributors', maxZoom: 20, subdomains: 'abcd' }
    }
  }
  const def = defs[key] || defs['carto']
  // before adding layer, probe a sample tile to ensure the service is reachable
  try {
    if (key && key !== 'osm') {
      // build a sample tile url (z=2,x=1,y=1) using first subdomain 'a'
      let sample = def.url.replace('{s}', 'a').replace('{z}', '2').replace('{x}', '1').replace('{y}', '1')
      sample = sample.replace('{r}', '')
      const ok = await probeTileUrl(sample)
      if (!ok) {
        console.warn(`Tile probe failed for ${key}, falling back to osm`)
        basemap.value = 'osm'
        return
      }
    }
  } catch (e) { console.warn('tile probe error', e) }
  if (tileLayerRef) { try { map.removeLayer(tileLayerRef) } catch (e) {} }
  // reset error counter for this key
  tileErrors[key] = 0

  tileLayerRef = L.tileLayer(def.url, def.opts)
  // store a custom key so we can detect current layer
  tileLayerRef.options._key = key
  tileLayerRef.addTo(map)
  // handle tile errors: increment counter and fallback to OSM when threshold exceeded
  tileLayerRef.on('tileerror', (err) => {
    try {
      tileErrors[key] = (tileErrors[key] || 0) + 1
      console.warn('tile error for', key, err, 'count=', tileErrors[key])
      if (tileErrors[key] >= TILE_ERROR_THRESHOLD && key !== 'osm') {
        console.warn(`Tile errors for ${key} reached ${TILE_ERROR_THRESHOLD}, falling back to osm`)
        // switch to osm as fallback
        basemap.value = 'osm'
      }
    } catch (e) { console.error(e) }
  })
  // ensure map renders tiles correctly after layer change
  setTimeout(() => { try { map.invalidateSize() } catch (e) {} }, 200)
}

function playStep() {
  const pts = plotTracks._points || []
  if (!pts || pts.length === 0) return
  playIndex = Math.min(playIndex, pts.length - 1)
  const p = pts[playIndex]
  playMarker.setLatLng(p)
  playIndex++
  if (playIndex >= pts.length) { clearInterval(playTimer); playTimer = null; playing = false }
}

function playPause(){
  if (!plotTracks._points || plotTracks._points.length === 0) return
  if (playing) {
    clearInterval(playTimer); playTimer = null; playing = false
  } else {
    playTimer = setInterval(playStep, 700)
    playing = true
  }
}

function resetPlay(){
  if (playTimer) { clearInterval(playTimer); playTimer = null }
  playIndex = 0
  playing = false
  if (plotTracks._points && plotTracks._points.length > 0 && playMarker) {
    playMarker.setLatLng(plotTracks._points[0])
  }
}
</script>

<style scoped>
#map { border-radius:8px }

.start-dot, .end-dot{
  width:14px;height:14px;border-radius:50%;display:block;border:3px solid #fff;box-shadow:0 0 0 3px rgba(255,255,255,0.2) inset}
.start-marker .start-dot{background:#ff4d4f;border-color:#fff;box-shadow:0 0 0 3px rgba(255,77,79,0.12)}
.end-marker .end-dot{background:#1f73ff;border-color:#fff;box-shadow:0 0 0 3px rgba(31,115,255,0.12)}

.leaflet-container .leaflet-marker-icon.start-marker{transform:translate(-9px,-9px)}
.leaflet-container .leaflet-marker-icon.end-marker{transform:translate(-9px,-9px)}

.leaflet-container .leaflet-marker-icon { box-shadow: 0 2px 6px rgba(0,0,0,0.15); }

</style>
