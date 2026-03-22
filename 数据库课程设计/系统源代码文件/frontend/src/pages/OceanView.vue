<template>
  <div class="ocean-page" style="padding:16px;display:flex;gap:16px">
    <div style="width:340px">
      <el-card>
        <h3>海洋参数筛选</h3>
        <div style="display:flex;flex-direction:column;gap:10px;margin-top:8px">
          <div style="display:flex;flex-direction:column;gap:6px">
            <label style="font-size:12px;color:#606266">开始时间</label>
            <el-date-picker v-model="startTime" type="datetime" placeholder="开始时间" style="width:100%" />
          </div>
          <div style="display:flex;flex-direction:column;gap:6px">
            <label style="font-size:12px;color:#606266">结束时间</label>
            <el-date-picker v-model="endTime" type="datetime" placeholder="结束时间" style="width:100%" />
          </div>
          <el-input v-model.number="bbox.min_lat" placeholder="纬度最小" />
          <el-input v-model.number="bbox.max_lat" placeholder="纬度最大" />
          <el-input v-model.number="bbox.min_lng" placeholder="经度最小" />
          <el-input v-model.number="bbox.max_lng" placeholder="经度最大" />
          <div style="display:flex;gap:8px;align-items:center">
            <el-button type="primary" @click="fetchEnv">查询海洋参数</el-button>
            <el-button @click="exportCsv">导出 CSV</el-button>
          </div>

          <div style="display:flex;flex-direction:column;gap:8px;margin-top:6px">
            <el-switch v-model="useCluster" active-text="点聚合" inactive-text="点聚合" />
            <el-switch v-model="useHeatmap" active-text="热力图" inactive-text="热力图" />
          </div>
        </div>
      </el-card>
    </div>

    <div style="flex:1;display:flex;flex-direction:column;gap:12px">
      <div style="height:420px;position:relative">
        <div id="ocean-map" style="width:100%;height:100%;border-radius:8px"></div>
      </div>

      <el-card style="height:260px;padding:12px">
        <div style="width:100%;height:100%">
          <div id="ocean-timeseries" style="width:100%;height:100%"></div>
        </div>
      </el-card>

          <el-card>
            <h4>查询结果</h4>
                <div>
                  <el-table :data="tableData" style="min-width:1400px" :height="240">
                <el-table-column prop="DataID" label="编号" width="80" />
                <el-table-column prop="ShipID" label="船舶ID" width="100" />
                <el-table-column prop="DateTime_u" label="时间" width="160" />
                <el-table-column prop="WaveHeight" label="浪高" width="100" />
                <el-table-column prop="SeaTemp" label="海温" width="100" />
                <el-table-column prop="SurgeDirection" label="涌流方向" width="110" />
                <el-table-column prop="WavePeriod" label="波周期" width="100" />
                <el-table-column prop="SurgeHeight" label="涌流高度" width="100" />
                <el-table-column prop="Lat" label="纬度" width="120" />
                <el-table-column prop="Long_u" label="经度" width="120" />
              </el-table>
            </div>
          </el-card>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch, nextTick } from 'vue'
import 'leaflet/dist/leaflet.css'
import L from 'leaflet'
import { getOceanEnvironment, getShips } from '../api'
import dayjs from 'dayjs'

const startTime = ref(null)
const endTime = ref(null)
const bbox = ref({ min_lat: 29.5, max_lat: 30.5, min_lng: 119.5, max_lng: 120.5 })
const tableData = ref([])
const ships = ref([])

// toggles and layers
const useCluster = ref(false)
const useHeatmap = ref(false)
let dataLayer = null
let clusterLayer = null
let heatLayer = null
let markersList = []

let map = null
let tsChartInstance = null

onMounted(async () => {
  try {
    const r = await getShips()
    ships.value = (r.data && r.data.ships) || []
  } catch (e) {}

  map = L.map('ocean-map').setView([30.0, 120.0], 6)
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; OpenStreetMap contributors'
  }).addTo(map)

  // (removed custom horizontal-wheel-to-scroll behavior)
})

// runtime loader for optional external libs (CDN)
function loadCss(url) {
  return new Promise((resolve, reject) => {
    try {
      const link = document.createElement('link')
      link.rel = 'stylesheet'
      link.href = url
      link.onload = () => resolve()
      link.onerror = () => reject(new Error('CSS load error ' + url))
      document.head.appendChild(link)
    } catch (e) { reject(e) }
  })
}

function loadScript(url) {
  return new Promise((resolve, reject) => {
    try {
      // avoid loading twice
      const exists = Array.from(document.scripts).find(s => s.src && s.src.indexOf(url.split('?')[0]) !== -1)
      if (exists) { resolve(); return }
      const s = document.createElement('script')
      s.src = url
      s.async = true
      s.onload = () => resolve()
      s.onerror = () => reject(new Error('Script load error ' + url))
      document.head.appendChild(s)
    } catch (e) { reject(e) }
  })
}

watch([useCluster, useHeatmap], () => {
  // if there is existing data, re-render points according to toggles
  if (tableData.value && tableData.value.length > 0) renderPoints(tableData.value)
})

function normalizeRow(d) {
  return {
    DataID: d.DataID ?? d.dataid ?? d.id ?? d.data_id ?? null,
    ShipID: d.ShipID ?? d.ship_id ?? d.ShipId ?? d.ship_id ?? null,
    DateTime_u: d.DateTime_u ?? d.DateTime ?? d.datetime ?? d.datetime ?? null,
    WaveHeight: d.WaveHeight ?? d.wave_height ?? d.Wave_Height ?? d.wave_height ?? null,
    SeaTemp: d.SeaTemp ?? d.sea_temp ?? d.Temperature ?? d.sea_temperature ?? null,
    SurgeDirection: d.SurgeDirection ?? d.surge_direction ?? d.Surge_Direction ?? d.surge_direction ?? null,
    WavePeriod: d.WavePeriod ?? d.wave_period ?? d.Wave_Period ?? d.wave_period ?? null,
    SurgeHeight: d.SurgeHeight ?? d.surge_height ?? d.surge_height ?? d.surge_height ?? null,
    Lat: d.Lat ?? d.lat ?? d.Lat_u ?? d.lat_u ?? d.latitude ?? null,
    Lat_u: d.Lat_u ?? d.Lat ?? d.latitude ?? null,
    Long_u: d.Long_u ?? d.Long ?? d.Longitude ?? d.lon ?? d.longitude ?? null,
    Long: d.Long ?? d.Long_u ?? d.longitude ?? null
  }
}

// try to import a library from local node_modules first, fallback false if it fails
async function ensureLocalImport(specifier) {
  try {
    // use explicit literal imports so Vite can analyze them
    if (specifier === 'leaflet.markercluster') {
      await import('leaflet.markercluster')
      return true
    }
    if (specifier === 'leaflet.heat') {
      await import('leaflet.heat')
      return true
    }
    if (specifier === 'echarts') {
      await import('echarts')
      return true
    }
    return false
  } catch (e) {
    return false
  }
}

async function fetchEnv() {
  const params = {}
  if (startTime.value) params.start_time = dayjs(startTime.value).format('YYYY-MM-DD HH:mm:ss')
  if (endTime.value) params.end_time = dayjs(endTime.value).format('YYYY-MM-DD HH:mm:ss')
  params.min_lat = bbox.value.min_lat
  params.max_lat = bbox.value.max_lat
  params.min_lng = bbox.value.min_lng
  params.max_lng = bbox.value.max_lng

  try {
    const r = await getOceanEnvironment(params)
    const data = (r.data && r.data.data) || []
    tableData.value = data.map(normalizeRow)
    renderPoints(tableData.value)
  } catch (e) { console.error(e) }
}

async function renderPoints(rows) {
  // clear existing
  try { if (dataLayer) { map.removeLayer(dataLayer); dataLayer = null } } catch(e){}
  try { if (clusterLayer) { map.removeLayer(clusterLayer); clusterLayer = null } } catch(e){}
  try { if (heatLayer) { map.removeLayer(heatLayer); heatLayer = null } } catch(e){}
  markersList = []

  dataLayer = L.layerGroup()
  const heatData = []

  // build series index for per-point/per-ship charts
  const seriesByShip = {}
  const seriesByCoord = {}

  rows.forEach(r => {
    // coerce numeric coords
    const lat = r.Lat != null ? Number(r.Lat) : NaN
    const lon = r.Long != null ? Number(r.Long) : NaN
    if (Number.isNaN(lat) || Number.isNaN(lon)) return

    // collect series keyed by ship and by coordinate
    const time = r.DateTime_u
    const val = r.WaveHeight != null ? Number(r.WaveHeight) : null
    if (r.ShipID) {
      seriesByShip[r.ShipID] = seriesByShip[r.ShipID] || []
      if (time && val != null) seriesByShip[r.ShipID].push({ time, value: val })
    }
    const coordKey = `${lat.toFixed(6)},${lon.toFixed(6)}`
    seriesByCoord[coordKey] = seriesByCoord[coordKey] || []
    if (time && val != null) seriesByCoord[coordKey].push({ time, value: val })

    const popup = `<div style="min-width:260px;line-height:1.4;font-size:13px"><strong>编号:</strong> ${r.DataID || ''}<br/><strong>船舶ID:</strong> ${r.ShipID || ''}<br/><strong>时间:</strong> ${r.DateTime_u || ''}<br/><strong>经度:</strong> ${lon}<br/><strong>纬度:</strong> ${lat}${r.WaveHeight != null ? '<br/><strong>浪高:</strong> ' + r.WaveHeight : ''}${r.SeaTemp != null ? '<br/><strong>海温:</strong> ' + r.SeaTemp : ''}${r.SurgeDirection ? '<br/><strong>涌流方向:</strong> ' + r.SurgeDirection : ''}${r.WavePeriod != null ? '<br/><strong>波周期:</strong> ' + r.WavePeriod : ''}${r.SurgeHeight != null ? '<br/><strong>涌流高度:</strong> ' + r.SurgeHeight : ''}</div>`

    const m = L.circleMarker([lat, lon], { radius:6, color:'#2f8cff', fillColor:'#fff', weight:2 })
    m.bindPopup(popup)
    m.on('mouseover', () => m.openPopup())
    m.on('mouseout', () => m.closePopup())
    // on click, render charts for this ship if available, otherwise for this coord
    m.on('click', () => {
      const shipKey = r.ShipID
      const keyCoord = coordKey
      const s = (shipKey && seriesByShip[shipKey] && seriesByShip[shipKey].length > 0) ? seriesByShip[shipKey] : (seriesByCoord[keyCoord] || [])
      if (s && s.length > 0) renderCharts(s)
    })

    markersList.push(m)
    // heat item
    const intensity = (r.WaveHeight != null) ? Number(r.WaveHeight) : 0
    heatData.push([lat, lon, Math.max(0, intensity)])
  })

  // draw polyline optionally (if sequence exists)
  const latlngs = rows.map(r => (r.Lat != null && r.Long != null) ? [r.Lat, r.Long] : null).filter(Boolean)
  if (latlngs.length > 0) {
    try {
      const polyBg = L.polyline(latlngs, { color: '#f0f6ff', weight: 8, opacity: 0.9 })
      const poly = L.polyline(latlngs, { color: '#2f8cff', weight: 3, opacity: 1 })
      polyBg.addTo(dataLayer)
      poly.addTo(dataLayer)
    } catch(e){}
  }

  // apply cluster or raw markers
  if (useCluster.value) {
    try {
      // prefer local package if available (avoids CDN/tracker blocking)
      const okLocal = await ensureLocalImport('leaflet.markercluster')
      if (okLocal && typeof L.markerClusterGroup === 'function') {
        clusterLayer = L.markerClusterGroup()
        markersList.forEach(m => clusterLayer.addLayer(m))
        clusterLayer.addTo(map)
      } else {
        // fallback to CDN
        await loadCss('https://unpkg.com/leaflet.markercluster@1.5.3/dist/MarkerCluster.css')
        await loadCss('https://unpkg.com/leaflet.markercluster@1.5.3/dist/MarkerCluster.Default.css')
        await loadScript('https://unpkg.com/leaflet.markercluster@1.5.3/dist/leaflet.markercluster.js')
        clusterLayer = L.markerClusterGroup()
        markersList.forEach(m => clusterLayer.addLayer(m))
        clusterLayer.addTo(map)
      }
    } catch (e) {
      console.warn('markercluster load failed, falling back to raw markers', e)
      markersList.forEach(m => m.addTo(dataLayer))
    }
  } else {
    markersList.forEach(m => m.addTo(dataLayer))
  }

  // heatmap
  if (useHeatmap.value) {
    try {
      const okHeatLocal = await ensureLocalImport('leaflet.heat')
      if (!okHeatLocal) {
        await loadScript('https://unpkg.com/leaflet.heat/dist/leaflet-heat.js')
      }
      // create heat layer only when map has non-zero size and we have data
      const createHeat = () => {
        try {
          if (!heatData || heatData.length === 0) return false
          const size = map && map.getSize ? map.getSize() : null
          if (!size || size.x === 0 || size.y === 0) return false
          if (heatLayer) { try { map.removeLayer(heatLayer) } catch (e) {} }
          heatLayer = L.heatLayer(heatData, { radius: 25, blur: 15, maxZoom: 12 }).addTo(map)
          return true
        } catch (err) {
          console.warn('heat create error', err)
          return false
        }
      }

      let ok = createHeat()
      let attempts = 0
      while (!ok && attempts < 6) {
        await new Promise(r => setTimeout(r, 250))
        attempts++
        ok = createHeat()
      }
      if (!ok) console.warn('leaflet.heat: could not create heat layer (map size or data invalid)')
    } catch (e) { console.warn('leaflet.heat load failed', e) }
  }

  dataLayer.addTo(map)
  try { const all = useCluster.value && clusterLayer ? clusterLayer.getLayers() : dataLayer.getLayers(); const group = L.featureGroup(all); map.fitBounds(group.getBounds(), { padding:[40,40] }) } catch (e) {}

  // render charts for overall dataset (if useful)
  const timeSeries = rows.map(r => ({ time: r.DateTime_u, value: r.WaveHeight })).filter(x => x.time && x.value != null)
  // initially show aggregate timeseries; user can click a marker to show that point/ship
  if (timeSeries.length > 0) renderCharts(timeSeries)
}

async function renderCharts(seriesData) {
  try {
    // prefer global if loaded, otherwise load from CDN
    let echarts = null
    try {
      const mod = await import('echarts')
      echarts = mod && (mod.default || mod)
    } catch (err) {
      if (typeof window !== 'undefined' && window.echarts) echarts = window.echarts
      else {
        await loadScript('https://cdn.jsdelivr.net/npm/echarts@5.4.2/dist/echarts.min.js')
        echarts = window.echarts
      }
    }
    // timeseries
    const tsDom = document.getElementById('ocean-timeseries')
    if (tsDom) {
      try {
        const sorted = (seriesData || []).slice().sort((a,b) => new Date(a.time) - new Date(b.time))
        const times = sorted.map(s => s.time)
        const vals = sorted.map(s => s.value)
        const opt = {
          title: { text: '浪高时序图', left: 'center', textStyle:{fontSize:12} },
          tooltip: { trigger: 'axis' },
          xAxis: { type: 'category', data: times },
          yAxis: { type: 'value', name: '浪高' },
          series: [{ type: 'line', data: vals, smooth: true, areaStyle: {} }]
        }
        // reuse existing instance if present
        try {
          const existing = echarts.getInstanceByDom(tsDom)
          if (existing) {
            existing.setOption(opt, true)
            tsChartInstance = existing
          } else {
            if (tsChartInstance) try { echarts.dispose(tsChartInstance) } catch(e){}
            tsChartInstance = echarts.init(tsDom)
            tsChartInstance.setOption(opt)
          }
        } catch (ex) {
          // fallback
          if (tsChartInstance) try { echarts.dispose(tsChartInstance) } catch(e){}
          tsChartInstance = echarts.init(tsDom)
          tsChartInstance.setOption(opt)
        }
      } catch (err) { console.warn('timeseries render failed', err) }
    }

        // boxplot removed — no action
  } catch (e) {
    console.warn('ECharts not available or render failed', e)
  }
}

function exportCsv() {
  if (!tableData.value || tableData.value.length === 0) return
  const headers = Object.keys(tableData.value[0])
  const rows = tableData.value.map(r => headers.map(h => (r[h] != null ? String(r[h]) : '')))
  const csv = [headers.join(',')].concat(rows.map(r => r.map(c => '"' + c.replace(/"/g,'""') + '"').join(','))).join('\n')
  const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' })
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = `ocean_params_${Date.now()}.csv`
  document.body.appendChild(a)
  a.click()
  document.body.removeChild(a)
  URL.revokeObjectURL(url)
}
</script>

<style scoped>
.ocean-page h3{margin:0 0 8px}
</style>
