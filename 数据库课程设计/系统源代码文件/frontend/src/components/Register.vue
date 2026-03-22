<template>
  <div class="register-container">
    <div class="register-box">
      <div class="logo-section">
        <img src="../../logo.jpg" alt="Logo" class="logo-img" />
        <h2 class="register-title">注册账号</h2>
      </div>
      <el-form :model="registerForm" :rules="rules" ref="registerFormRef" class="register-form">
        
          <el-form-item prop="username">
            <el-input v-model="registerForm.username" placeholder="请输入用户名" size="large">
              <template #prefix>
                <el-icon>
                  <component :is="User" />
                </el-icon>
              </template>
            </el-input>
          </el-form-item>
        
          <el-form-item prop="email">
            <el-input v-model="registerForm.email" placeholder="请输入邮箱" size="large">
              <template #prefix>
                <el-icon>
                  <component :is="Message" />
                </el-icon>
              </template>
            </el-input>
          </el-form-item>
        
          <el-form-item prop="verificationCode">
            <div style="display: flex; gap: 10px;">
              <el-input
                v-model="registerForm.verificationCode"
                placeholder="请输入验证码"
                size="large"
                style="flex: 1;"
              >
                <template #prefix>
                  <el-icon>
                    <component :is="Key" />
                  </el-icon>
                </template>
              </el-input>
              <el-button
                type="primary"
                size="large"
                :loading="codeLoading"
                :disabled="!registerForm.email || countdown > 0"
                @click="handleSendCode"
              >
                {{ countdown > 0 ? `${countdown}秒后重发` : '发送验证码' }}
              </el-button>
            </div>
          </el-form-item>
        
          <el-form-item prop="password">
            <el-input v-model="registerForm.password" type="password" placeholder="请输入密码（至少6位）" size="large">
              <template #prefix>
                <el-icon>
                  <component :is="Lock" />
                </el-icon>
              </template>
            </el-input>
          </el-form-item>
        
          <el-form-item prop="confirmPassword">
            <el-input v-model="registerForm.confirmPassword" type="password" placeholder="请确认密码" size="large" @keyup.enter="handleRegister">
              <template #prefix>
                <el-icon>
                  <component :is="Lock" />
                </el-icon>
              </template>
            </el-input>
          </el-form-item>
        <el-form-item>
          <el-button type="primary" size="large" class="register-button" :loading="loading" @click="handleRegister">
            注册
          </el-button>
        </el-form-item>
      </el-form>
      <div class="login-link">
        已有账号？
        <el-link type="primary" @click="$router.push('/login')">立即登录</el-link>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import axios from 'axios'
import { ElMessage } from 'element-plus'
import { User, Message, Key, Lock } from '@element-plus/icons-vue'

const router = useRouter()

const registerFormRef = ref(null)
const loading = ref(false)
const codeLoading = ref(false)
const countdown = ref(0)

const registerForm = ref({
  username: '',
  email: '',
  verificationCode: '',
  password: '',
  confirmPassword: ''
})

const validatePassword = (rule, value, callback) => {
  if (value !== registerForm.value.password) {
    callback(new Error('两次输入的密码不一致'))
  } else {
    callback()
  }
}

const rules = {
  username: [{ required: true, message: '请输入用户名', trigger: 'blur' }],
  email: [
    { required: true, message: '请输入邮箱', trigger: 'blur' },
    { validator: (rule, value, callback) => {
      const re = /^[^\s@]+@[^\s@]+\.[^\s@]+$/
      if (!value || !re.test(value)) callback(new Error('邮箱格式不正确'))
      else callback()
    }, trigger: 'blur' }
  ],
  verificationCode: [{ required: true, message: '请输入验证码', trigger: 'blur' }],
  password: [
    { required: true, message: '请输入密码', trigger: 'blur' },
    { min: 6, message: '密码至少6位', trigger: 'blur' }
  ],
  confirmPassword: [{ validator: validatePassword, trigger: 'blur' }]
}

const handleSendCode = async () => {
  if (!registerForm.value.email) {
    ElMessage.warning('请先输入邮箱')
    return
  }
  codeLoading.value = true
  try {
    ElMessage.success('模拟发送验证码（开发阶段）')
    countdown.value = 60
    const timer = setInterval(() => {
      countdown.value--
      if (countdown.value <= 0) clearInterval(timer)
    }, 1000)
  } catch (err) {
    ElMessage.error('发送验证码失败')
  } finally {
    codeLoading.value = false
  }
}

const handleRegister = async () => {
  if (!registerFormRef.value) return
  try {
    // 表单校验
    await registerFormRef.value.validate()
  } catch (e) {
    return
  }
  try {
    loading.value = true
    const r = await axios.post('/api/user/register', {
      email: registerForm.value.email,
      password: registerForm.value.password,
      name: registerForm.value.username,
      role: 0
    })
    loading.value = false
    if (r.status === 200 && !(r.data && r.data.error)) {
      ElMessage.success(r.data?.message || '注册成功，请登录')
      router.push('/login')
    } else {
      ElMessage.error(r.data?.error || r.data?.message || '注册失败')
    }
  } catch (err) {
    loading.value = false
    ElMessage.error(err.response?.data?.error || err.message || '请求失败')
  }
}
</script>

<style scoped>
.register-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background: linear-gradient(135deg, #1890ff 0%, #096dd9 50%, #0050b3 100%);
  padding: 20px;
}

.register-box {
  width: 420px;
  min-height: 720px;
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
  margin-bottom: 35px;
}

.logo-img {
  width: 80px;
  height: 80px;
  object-fit: contain;
  margin-bottom: 20px;
  border-radius: 8px;
}

.register-title {
  text-align: center;
  font-size: 26px;
  font-weight: 600;
  color: #1890ff;
  margin: 0;
  letter-spacing: 0.5px;
}

.register-form {
  flex: 1;
  margin-bottom: 24px;
}

.register-form :deep(.el-form-item) {
  margin-bottom: 22px;
}

.register-form :deep(.el-input__wrapper) {
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(24, 144, 255, 0.08);
  transition: all 0.3s;
}

.register-form :deep(.el-input__wrapper:hover) {
  box-shadow: 0 4px 12px rgba(24, 144, 255, 0.15);
}

.register-form :deep(.el-input.is-focus .el-input__wrapper) {
  box-shadow: 0 4px 12px rgba(24, 144, 255, 0.2);
}

.register-form :deep(.el-button) {
  border-radius: 8px;
}

.register-button {
  width: 100%;
  height: 48px;
  font-size: 16px;
  font-weight: 500;
  border-radius: 8px;
  background: linear-gradient(135deg, #1890ff 0%, #096dd9 100%);
  border: none;
  transition: all 0.3s;
  margin-top: 8px;
}

.register-button:hover {
  background: linear-gradient(135deg, #096dd9 0%, #0050b3 100%);
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(24, 144, 255, 0.3);
}

.login-link {
  text-align: center;
  color: #595959;
  font-size: 14px;
  margin-top: auto;
  padding-top: 20px;
}

.login-link :deep(.el-link) {
  color: #1890ff;
  font-weight: 500;
}

.login-link :deep(.el-link:hover) {
  color: #096dd9;
}
</style>
