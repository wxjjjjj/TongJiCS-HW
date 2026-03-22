<template>
  <div class="layout-root">
    <el-container style="min-height:100vh">
      <el-aside width="240px" v-show="showLayout" class="site-aside">
        <div class="brand-bar">
          <div class="brand">船舶轨迹查询及海洋环境分析平台</div>
        </div>

        <div class="menu-box">
          <el-menu :default-active="$route.path" class="el-menu-vertical-demo" router>
            <el-menu-item index="/dashboard">
              <template #icon>
                <el-icon><component :is="UserIcon" /></el-icon>
              </template>
              个人中心
            </el-menu-item>
            <el-menu-item index="/map">
              <template #icon>
                <el-icon><component :is="MapIcon" /></el-icon>
              </template>
              船舶轨迹
            </el-menu-item>
            <el-menu-item index="/ocean">
              <template #icon>
                <el-icon><component :is="WaterIcon" /></el-icon>
              </template>
              海洋参数
            </el-menu-item>
            <el-menu-item index="/admin" v-if="isAdmin">
              <template #icon>
                <el-icon><component :is="AdminIcon" /></el-icon>
              </template>
              管理员面板
            </el-menu-item>
          </el-menu>
        </div>

        <div class="user-pod" v-if="user">
          <div class="user-info">
            <el-icon style="vertical-align:middle;margin-right:8px"><component :is="UserIcon" /></el-icon>
            <span class="user-name-text">{{ user ? (user.name || user.email) : '' }}</span>
          </div>
          <el-button type="danger" size="small" plain @click="logout" class="logout-btn">退出登录</el-button>
        </div>
      </el-aside>

      <el-container :class="{ 'with-aside': showLayout }">
        <el-header v-show="showLayout" class="site-header">
          <div class="header-left">{{ pageTitle }}</div>
        </el-header>

        <el-main class="site-main">
          <router-view />
        </el-main>
      </el-container>
    </el-container>
  </div>
</template>

<script setup>
import { computed, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { useUserStore } from '../stores/user'
import { User, Location, Tools } from '@element-plus/icons-vue'

const route = useRoute()
const router = useRouter()

const hidePaths = ['/login','/register']
const showLayout = computed(() => !hidePaths.includes(route.path))

const userStore = useUserStore()
onMounted(() => { userStore.restoreFromStorage() })
const user = computed(() => userStore.user)
const isAdmin = computed(() => userStore.isAdmin)

// icon components
const UserIcon = User
const MapIcon = Location
// `Watermark` export doesn't exist in some icon sets; reuse `Location` as a safe placeholder
const WaterIcon = Location
const AdminIcon = Tools

const pageTitle = computed(() => {
  return route.meta && route.meta.title ? route.meta.title : '船舶轨迹查询及海洋环境分析平台'
})

function logout(){
  userStore.clearUser()
  router.push('/login')
}

const title = '船舶轨迹查询及海洋环境分析平台'
</script>

<style scoped>
.site-aside{background:linear-gradient(135deg, #1890ff 0%, #096dd9 50%, #0050b3 100%);padding:12px}
.brand{color:#fff;font-weight:700;padding:8px 0;margin-bottom:12px}
.user-box{position:absolute;bottom:16px;left:16px;right:16px}
.user-name{color:#fff;margin-bottom:8px}
.site-header{background:#fff;box-shadow:0 1px 0 rgba(0,0,0,0.06);display:flex;align-items:center;padding:0 16px}
.site-main{padding:16px;background:#f5f7fb;min-height:calc(100vh - 64px);overflow:auto}

/* make aside fixed so only right side scrolls */
.site-aside{position:fixed;top:0;left:0;bottom:0;width:240px;z-index:20;overflow:auto;display:flex;flex-direction:column}

.brand-bar{height:64px;display:flex;align-items:center;padding:0 12px}
.brand-bar .brand{font-size:16px}

.menu-box{background:#fff;border-radius:6px;padding:8px;margin:8px}
.el-menu-vertical-demo{background:transparent;border:none}
.el-menu-vertical-demo .el-menu-item{border-radius:6px;margin:6px 0;padding-left:8px}
.el-menu-vertical-demo .el-menu-item.is-active{background:#eaf6ff;color:#1890ff}
.el-menu-vertical-demo .el-menu-item:hover{background:rgba(24,144,255,0.06)}

.user-pod{margin-top:auto;padding:18px}
.user-info{background:#fff;padding:10px;border-radius:8px;display:flex;align-items:center;margin-bottom:8px}
.user-name-text{color:#0b66d1;font-weight:600}
.logout-btn{width:100%;background:#fff;border-color:#ff7b79;color:#ff4d4f}

/* when aside is shown, offset header/main to avoid overlap */
.with-aside .site-header{margin-left:240px}
.with-aside .site-main{margin-left:240px;padding-top:20px}
</style>
