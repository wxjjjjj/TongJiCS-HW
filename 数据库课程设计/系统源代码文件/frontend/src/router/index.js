import { createRouter, createWebHistory } from 'vue-router'
import { useUserStore } from '../stores/user'
import Login from '../components/Login.vue'
import Register from '../components/Register.vue'
import Dashboard from '../pages/Dashboard.vue'
import MapView from '../pages/MapView.vue'
import OceanView from '../pages/OceanView.vue'
import AdminPanel from '../pages/AdminPanel.vue'

const routes = [
  { path: '/', redirect: '/dashboard' },
  { path: '/login', component: Login },
  { path: '/register', component: Register },
  { path: '/dashboard', component: Dashboard, meta: { requiresAuth: true, title: '个人中心' } },
  { path: '/map', component: MapView, meta: { requiresAuth: true, title: '船舶轨迹' } },
  { path: '/ocean', component: OceanView, meta: { requiresAuth: true, title: '海洋参数' } },
  { path: '/admin', component: AdminPanel, meta: { requiresAuth: true, requiresAdmin: true, title: '管理员面板' } }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

// Simple route guard using Pinia store (restore from localStorage if needed)
router.beforeEach((to, from, next) => {
  try {
    // lazy import store because router file executes before app pinia init in some setups
    const userStore = useUserStore()
    if (!userStore.user) userStore.restoreFromStorage()
    const user = userStore.user
    if (to.meta.requiresAuth && !user) return next('/login')
    if (to.meta.requiresAdmin && (!user || user.role !== 1)) return next('/dashboard')
  } catch (e) {
    // fallback to localStorage if store not available
    const userRaw = localStorage.getItem('user')
    const user = userRaw ? JSON.parse(userRaw) : null
    if (to.meta.requiresAuth && !user) return next('/login')
    if (to.meta.requiresAdmin && (!user || user.role !== 1)) return next('/dashboard')
  }
  next()
})

export default router
