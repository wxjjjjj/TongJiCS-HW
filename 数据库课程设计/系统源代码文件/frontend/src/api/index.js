import axios from 'axios'

const api = axios.create({
  baseURL: '/api',
  timeout: 10000
})

export async function getShips() {
  return api.get('/data/ships')
}

export async function getVesselTracks(params) {
  // params: { ship_id, start_time, end_time }
  return api.get('/data/vessel-tracks', { params })
}

export async function getOceanEnvironment(params) {
  return api.get('/data/ocean-environment', { params })
}

export async function addData(payload, headers = {}) {
  return api.post('/data/add', payload, { headers })
}

export async function importCsv(formData, headers = {}) {
  // let axios set multipart headers (boundary) — don't set Content-Type manually
  return api.post('/data/import', formData, { headers })
}

export async function getRecords(params, headers = {}) {
  return api.get('/record/operations', { params, headers })
}

export async function listData(params, headers = {}) {
  return api.get('/data/list', { params, headers })
}

export async function deleteData(payload, headers = {}) {
  return api.post('/data/delete', payload, { headers })
}

export async function updateData(payload, headers = {}) {
  return api.post('/data/update', payload, { headers })
}

export default api
