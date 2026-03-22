<template>
  <div class="login-container">
    <div class="login-box">
      <div class="logo-section">
        <img src="../../logo.jpg" alt="Logo" class="logo-img" />
        <h2 class="login-title">船舶轨迹查询及海洋环境分析平台</h2>
      </div>
      <el-form :model="loginForm" :rules="rules" ref="loginFormRef" class="login-form">
        <el-form-item>
          <el-radio-group v-model="loginForm.loginType" class="login-type-group">
            <el-radio-button label="user">用户登录</el-radio-button>
            <el-radio-button label="admin">管理员登录</el-radio-button>
          </el-radio-group>
        </el-form-item>

        <el-form-item prop="email">
          <el-input v-model="loginForm.email" placeholder="请输入邮箱" size="large">
            <template #prefix>
              <el-icon>
                <component :is="Message" />
              </el-icon>
            </template>
          </el-input>
        </el-form-item>

        <el-form-item prop="password">
          <el-input v-model="loginForm.password" type="password" placeholder="请输入密码" size="large" @keyup.enter="handleLogin">
            <template #prefix>
              <el-icon>
                <component :is="Lock" />
              </el-icon>
            </template>
          </el-input>
        </el-form-item>

        <el-form-item>
          <el-button type="primary" size="large" class="login-button" :loading="loading" @click="handleLogin">
            登录
          </el-button>
        </el-form-item>
      </el-form>
      <div class="register-link" v-if="loginForm.loginType === 'user'">
        还没有账号？
        <el-link type="primary" @click="$router.push('/register')">立即注册</el-link>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import axios from 'axios'
import { ElMessage } from 'element-plus'
import { Message, Lock } from '@element-plus/icons-vue'
import { useUserStore } from '../stores/user'

const router = useRouter()
const userStore = useUserStore()

const loginFormRef = ref(null)
const loading = ref(false)

const loginForm = ref({
  loginType: 'user',
  email: '',
  password: ''
})

const rules = {
  email: [
    { required: true, message: '请输入邮箱', trigger: 'blur' },
    { validator: (rule, value, callback) => {
      const re = /^[^\s@]+@[^\s@]+\.[^\s@]+$/
      if (!value || !re.test(value)) callback(new Error('邮箱格式不正确'))
      else callback()
    }, trigger: 'blur' }
  ],
  password: [{ required: true, message: '请输入密码', trigger: 'blur' }]
}

const handleLogin = async () => {
  if (!loginFormRef.value) return
  try {
    await loginFormRef.value.validate()
  } catch (e) {
    return
  }

  try {
    loading.value = true
    const r = await axios.post('/api/user/login', {
      email: loginForm.value.email,
      password: loginForm.value.password
    })
    loading.value = false
    if (r.data && r.data.user) {
      userStore.setUser(r.data.user)
      ElMessage.success('登录成功')
      router.push('/dashboard')
    } else {
      ElMessage.error(r.data?.message || '登录失败')
    }
  } catch (err) {
    loading.value = false
    ElMessage.error(err.response?.data?.error || err.message || '请求失败')
  }
}
</script>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background: linear-gradient(135deg, #1890ff 0%, #096dd9 50%, #0050b3 100%);
  padding: 20px;
}

.login-box {
  width: 420px;
  min-height: 620px;
  padding: 50px 45px;
  background: #ffffff;
  border-radius: 16px;
  box-shadow: 0 8px 32px rgba(24, 144, 255, 0.15);
  display: flex;
  flex-direction: column;
}

.logo-section {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 40px;
}

.logo-img {
  width: 120px;
  height: 120px;
  object-fit: contain;
  margin-bottom: 20px;
  border-radius: 8px;
}

@media (max-width: 480px) {
  .logo-img {
    width: 96px;
    height: 96px;
  }
}

.login-title {
  text-align: center;
  font-size: 26px;
  font-weight: 600;
  color: #1890ff;
  margin: 0;
  letter-spacing: 0.5px;
}

.login-form {
  flex: 1;
  margin-bottom: 24px;
}

.login-form :deep(.el-form-item) {
  margin-bottom: 24px;
}

.login-form :deep(.el-input__wrapper) {
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(24, 144, 255, 0.08);
  transition: all 0.3s;
}

.login-form :deep(.el-input__wrapper:hover) {
  box-shadow: 0 4px 12px rgba(24, 144, 255, 0.15);
}

.login-form :deep(.el-input.is-focus .el-input__wrapper) {
  box-shadow: 0 4px 12px rgba(24, 144, 255, 0.2);
}

.login-type-group {
  width: 100%;
  display: flex;
  justify-content: center;
  margin-bottom: 8px;
}

.login-type-group :deep(.el-radio-button__inner) {
  padding: 12px 24px;
  font-size: 14px;
  border-radius: 8px;
}

.login-type-group :deep(.el-radio-button__original-radio:checked + .el-radio-button__inner) {
  background: linear-gradient(135deg, #1890ff 0%, #096dd9 100%);
  border-color: #1890ff;
}

.login-button {
  width: 100%;
  height: 48px;
  font-size: 16px;
  font-weight: 500;
  border-radius: 8px;
  background: linear-gradient(135deg, #1890ff 0%, #096dd9 100%);
  border: none;
  transition: all 0.3s;
}

.login-button:hover {
  background: linear-gradient(135deg, #096dd9 0%, #0050b3 100%);
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(24, 144, 255, 0.3);
}

.register-link {
  text-align: center;
  color: #595959;
  font-size: 14px;
  margin-top: auto;
  padding-top: 20px;
}

.register-link :deep(.el-link) {
  color: #1890ff;
  font-weight: 500;
}

.register-link :deep(.el-link:hover) {
  color: #096dd9;
}
</style>
