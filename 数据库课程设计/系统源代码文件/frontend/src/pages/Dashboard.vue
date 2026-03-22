<template>
  <div class="page-dashboard">
    <div class="dash-grid">
      <div class="left-card">
        <el-card>
          <h3>基本信息</h3>
          <el-descriptions column="1" border>
            <el-descriptions-item label="用户名">{{ user?.name || user?.email || '未登录' }}</el-descriptions-item>
            <el-descriptions-item label="邮箱">{{ user?.email || '-' }}</el-descriptions-item>
            <el-descriptions-item label="角色">{{ roleText }}</el-descriptions-item>
            <el-descriptions-item label="登录时间">{{ loginTime }}</el-descriptions-item>
          </el-descriptions>
        </el-card>
      </div>

      <div class="right-calendar">
        <el-card>
          <div class="calendar-header">
            <div class="month-title">{{ year }} 年 {{ month + 1 }} 月</div>
            <div class="today-note">今天：<strong>{{ todayLabel }}</strong></div>
          </div>
          <div class="calendar-grid">
            <div class="weekday" v-for="w in weekdays" :key="w">{{ w }}</div>
            <div v-for="(cell, idx) in cells" :key="idx" class="day-cell" :class="{ empty: !cell, today: isToday(cell) }">
              <span v-if="cell">{{ cell }}</span>
            </div>
          </div>
        </el-card>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import dayjs from 'dayjs'
import { useUserStore } from '../stores/user'

const userStore = useUserStore()
onMounted(() => { userStore.restoreFromStorage() })
const user = computed(() => userStore.user)

const now = dayjs()
const loginTime = ref(now.format('YYYY-MM-DD HH:mm:ss'))

const year = ref(now.year())
const month = ref(now.month()) // 0-based
const today = ref(now.date())

const weekdays = ['Sun','Mon','Tue','Wed','Thu','Fri','Sat']

function generateCells(y, m) {
  const first = new Date(y, m, 1)
  const startWeek = first.getDay()
  const daysInMonth = new Date(y, m + 1, 0).getDate()
  const cells = []
  for (let i = 0; i < startWeek; i++) cells.push(null)
  for (let d = 1; d <= daysInMonth; d++) cells.push(d)
  while (cells.length < 42) cells.push(null)
  return cells
}

const cells = ref(generateCells(year.value, month.value))

function isToday(day) {
  return day && year.value === now.year() && month.value === now.month() && day === now.date()
}

const roleText = computed(() => {
  if (!user.value) return '未登录'
  return user.value.role === 1 ? '管理员' : '普通用户'
})

const todayLabel = `${now.format('YYYY-MM-DD')}`
</script>

<style scoped>
.page-dashboard { padding: 24px }
.dash-grid{display:flex;gap:24px;align-items:flex-start}
.left-card{flex:0 0 58%}
.right-calendar{flex:0 0 38%}

/* 基本信息卡片样式 */
.left-card h3{color:#1890ff;font-size:20px;margin:0 0 12px;font-weight:700}
/* 使用深度选择器确保 Element Plus 内部类的样式在 scoped 中生效，增加优先级覆盖默认样式 */
.left-card :deep(.el-descriptions){font-size:18px}
.left-card :deep(.el-descriptions__label){color:#8892a6;font-weight:600;width:140px;font-size:15px}
.left-card :deep(.el-descriptions__content){color:#222;font-weight:700;font-size:18px}
.left-card :deep(.el-descriptions__item),
.left-card :deep(.el-descriptions__row),
.left-card :deep(td),
.left-card :deep(.el-descriptions__content *) {
  font-size:18px !important;
  line-height:1.6 !important;
  font-weight:700 !important;
}

/* Token 大数字样式（可选） */
.token-box{background:linear-gradient(90deg,#2f8cff,#1a6bff);color:#fff;padding:22px;border-radius:8px;text-align:center;margin-top:18px}
.token-box .num{font-size:36px;font-weight:800}

/* 日历样式 */
.calendar-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:18px}
.calendar-header .month-title{color:#1890ff;font-size:18px;font-weight:700}
.calendar-header .today-note{color:#666;font-size:13px}
.calendar-grid{display:grid;grid-template-columns:repeat(7,1fr);gap:8px}
.weekday{font-weight:700;color:#8c9bb3;text-align:center;font-size:13px}
.day-cell{height:72px;border-radius:6px;background:#f5f7fa;display:flex;align-items:center;justify-content:center;font-size:14px}
.day-cell.empty{background:transparent}
.day-cell.today{background:#1890ff;color:#fff;font-weight:800;box-shadow:0 2px 8px rgba(24,144,255,0.18)}

.el-card{padding:18px}

</style>
