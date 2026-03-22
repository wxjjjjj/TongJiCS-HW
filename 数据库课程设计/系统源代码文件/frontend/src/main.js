import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import { createPinia } from 'pinia'
import 'leaflet/dist/leaflet.css'
// markercluster css (optional visual polish if markercluster is used)
import 'leaflet.markercluster/dist/MarkerCluster.css'
import 'leaflet.markercluster/dist/MarkerCluster.Default.css'

const app = createApp(App)
const pinia = createPinia()
app.use(pinia).use(router).use(ElementPlus).mount('#app')
