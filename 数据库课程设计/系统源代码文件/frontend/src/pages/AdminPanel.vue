<template>
  <div style="padding:16px">
    <h3>管理员面板 — 添加单条数据</h3>
    <el-form :model="form" label-width="120px" style="max-width:640px">
      <el-form-item label="Ship ID"><el-input v-model="form.ship_id" /></el-form-item>
      <el-form-item label="Datetime"><el-input v-model="form.datetime" placeholder="YYYY-MM-DD HH:mm:ss"/></el-form-item>
      <el-form-item label="Lat"><el-input v-model="form.lat"/></el-form-item>
      <el-form-item label="Long"><el-input v-model="form.long"/></el-form-item>
      <el-form-item label="Sea Temp"><el-input v-model="form.sea_temp"/></el-form-item>
      <el-form-item label="Wave Height"><el-input v-model="form.wave_height"/></el-form-item>
      <el-form-item label="Wave Period"><el-input v-model="form.wave_period"/></el-form-item>
      <el-form-item label="Surge Dir"><el-input v-model="form.surge_direction"/></el-form-item>
      <el-form-item label="Surge Height"><el-input v-model="form.surge_height"/></el-form-item>
      <el-form-item>
        <el-button type="primary" @click="submit">提交</el-button>
      </el-form-item>
    </el-form>

    <el-divider />
    <h3>批量导入 CSV</h3>
    <div style="max-width:640px">
      <el-upload
        ref="uploader"
        action=""
        :before-upload="beforeUpload"
        :on-change="onFileChange"
        :auto-upload="false"
        :file-list="fileList">
        <el-button>选择 CSV 文件</el-button>
      </el-upload>
      <el-button type="primary" style="margin-left:8px" @click="uploadCsv">上传并导入</el-button>
      <div style="margin-top:8px;color:#666">注意：CSV 需包含列 ship_id, datetime, lat, long, sea_temp, wave_height, wave_period, surge_direction, surge_height</div>
    </div>

    <el-divider />
    <h3>导入/操作记录</h3>
    <el-table :data="records" style="width:100%" :height="320">
      <el-table-column prop="record_id" label="RecordID" width="90"/>
      <el-table-column prop="operation_type" label="类型" width="110"/>
      <el-table-column prop="datetime" label="时间" width="180"/>
      <el-table-column prop="user_id" label="UserID" width="90"/>
      <el-table-column prop="user_name" label="用户" width="140"/>
      <el-table-column prop="data_id" label="DataID" width="90"/>
      <el-table-column prop="ship_id" label="ShipID" width="90"/>
      <el-table-column prop="data_time" label="DataTime" width="180"/>
    </el-table>
    <div style="margin-top:8px;text-align:right">
      <el-pagination :page-size="perPage" :current-page.sync="page" :total="total" @current-change="fetchRecords"/>
    </div>
    
    <el-divider />
    <h3>数据库数据管理</h3>
    <el-button type="primary" @click="fetchDataList">刷新数据列表</el-button>
    <el-table :data="dataList" style="margin-top:12px;width:100%" :height="320">
      <el-table-column prop="DataID" label="DataID" width="90"/>
      <el-table-column prop="ShipID" label="ShipID" width="90"/>
      <el-table-column prop="DateTime_u" label="时间" width="180"/>
      <el-table-column prop="Lat" label="纬度" width="110"/>
      <el-table-column prop="Long_u" label="经度" width="110"/>
      <el-table-column prop="SeaTemp" label="海温" width="100"/>
      <el-table-column prop="WaveHeight" label="浪高" width="100"/>
      <el-table-column prop="WavePeriod" label="浪周期" width="100"/>
      <el-table-column prop="SurgeDirection" label="涌浪方向" width="110"/>
      <el-table-column prop="SurgeHeight" label="涌浪高度" width="110"/>
      <el-table-column label="操作" width="200">
        <template #default="{ row }">
          <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
          <el-button type="danger" size="small" style="margin-left:8px" @click="confirmDelete(row.DataID)">删除</el-button>
        </template>
      </el-table-column>
    </el-table>
    <div style="margin-top:8px;text-align:right">
      <el-pagination :page-size="dataPerPage" :current-page.sync="dataPage" :total="dataTotal" @current-change="fetchDataList"/>
    </div>

    <el-dialog title="编辑数据" v-model="editDialogVisible" append-to-body :close-on-click-modal="false" width="700px">
      <el-form :model="editForm" label-width="110px">
        <el-form-item label="ShipID"><el-input v-model="editForm.ShipID" /></el-form-item>
        <el-form-item label="时间"><el-input v-model="editForm.DateTime_u" /></el-form-item>
        <el-form-item label="纬度"><el-input v-model="editForm.Lat" /></el-form-item>
        <el-form-item label="经度"><el-input v-model="editForm.Long_u" /></el-form-item>
        <el-form-item label="海温"><el-input v-model="editForm.SeaTemp" /></el-form-item>
        <el-form-item label="浪高"><el-input v-model="editForm.WaveHeight" /></el-form-item>
        <el-form-item label="浪周期"><el-input v-model="editForm.WavePeriod" /></el-form-item>
        <el-form-item label="涌浪方向"><el-input v-model="editForm.SurgeDirection" /></el-form-item>
        <el-form-item label="涌浪高度"><el-input v-model="editForm.SurgeHeight" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="editDialogVisible=false">取消</el-button>
        <el-button type="primary" @click="submitEdit">保存</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { addData, importCsv, getRecords, listData, deleteData } from '../api'
import { updateData } from '../api'
import { ElMessage, ElMessageBox } from 'element-plus'

const form = ref({ ship_id:'', datetime:'', lat:'', long:'', sea_temp:'', wave_height:'', wave_period:'', surge_direction:'E', surge_height:'' })
const fileList = ref([])
const uploader = ref(null)
const records = ref([])
const page = ref(1)
const perPage = ref(20)
const total = ref(0)
const dataList = ref([])
const dataPage = ref(1)
const dataPerPage = ref(20)
const dataTotal = ref(0)
const editDialogVisible = ref(false)
const editForm = ref({ DataID: null, ShipID: '', DateTime_u: '', Lat: '', Long_u: '', SeaTemp: '', WaveHeight: '', WavePeriod: '', SurgeDirection: '', SurgeHeight: '' })

function beforeUpload(file) {
  // only allow csv
  const ok = file.type === 'text/csv' || file.name.toLowerCase().endsWith('.csv')
  if (!ok) ElMessage.error('请选择 CSV 文件')
  return ok
}

function onFileChange(file) {
  fileList.value = [file.raw || file]
}

async function fetchRecords(p = page.value) {
  const user = JSON.parse(localStorage.getItem('user') || 'null')
  const roleHeader = user && user.role === 1 ? '1' : '0'
  try {
    const r = await getRecords({ page: p, per_page: perPage.value }, { 'User-Role': roleHeader })
    if (r.data && r.data.success) {
      records.value = r.data.data
      total.value = r.data.pagination.total
    }
  } catch (e) { console.error(e) }
}

async function submit() {
  // 从 localStorage 读用户角色
  const user = JSON.parse(localStorage.getItem('user') || 'null')
  const headers = { 'User-Role': user && user.role === 1 ? '1' : '0', 'User-ID': user?.user_id || 0 }
  try {
    const r = await addData(form.value, headers)
    if (r.data && r.data.success) ElMessage.success('添加成功')
    else ElMessage.error(r.data?.error || '添加失败')
  } catch (e) { ElMessage.error(e.message || '请求失败') }
}

async function uploadCsv() {
  if (!fileList.value || fileList.value.length === 0) { ElMessage.error('请先选择文件'); return }
  const user = JSON.parse(localStorage.getItem('user') || 'null')
  const roleHeader = user && user.role === 1 ? '1' : '0'
  const fd = new FormData()
  fd.append('file', fileList.value[0])
  try {
    const headers = { 'User-Role': roleHeader, 'User-ID': user?.user_id || 0 }
    const r = await importCsv(fd, headers)
    if (r.data && r.data.success) {
      ElMessage.success(`导入完成：已导入 ${r.data.imported} 条，失败 ${r.data.failed.length} 条`)
      fetchRecords(1)
    } else {
      ElMessage.error(r.data?.error || '导入失败')
    }
  } catch (e) { ElMessage.error(e.message || '导入请求失败') }
}

// initial load
fetchRecords()
// fetch data list
fetchDataList()

async function fetchDataList(p = dataPage.value) {
  const user = JSON.parse(localStorage.getItem('user') || 'null')
  const roleHeader = user && user.role === 1 ? '1' : '0'
  try {
    const r = await listData({ page: p, per_page: dataPerPage.value }, { 'User-Role': roleHeader })
    if (r.data && r.data.success) {
      dataList.value = r.data.data
      dataTotal.value = r.data.pagination.total
    }
  } catch (e) { console.error(e) }
}

function confirmDelete(dataId) {
  ElMessageBox.confirm('确认删除 DataID=' + dataId + ' 吗？', '确认删除', { type:'warning' }).then(() => doDelete(dataId)).catch(()=>{})
}

async function doDelete(dataId) {
  const user = JSON.parse(localStorage.getItem('user') || 'null')
  const roleHeader = user && user.role === 1 ? '1' : '0'
  try {
    const r = await deleteData({ data_id: dataId }, { 'User-Role': roleHeader, 'User-ID': user?.user_id || 0 })
    if (r.data && r.data.success) {
      ElMessage.success('删除成功')
      fetchDataList()
    } else ElMessage.error(r.data?.error || '删除失败')
  } catch (e) { ElMessage.error(e.message || '删除请求失败') }
}

function openEdit(row) {
  console.log('openEdit', row)
  editForm.value = Object.assign({}, row)
  editDialogVisible.value = true
}

async function submitEdit() {
  const user = JSON.parse(localStorage.getItem('user') || 'null')
  const roleHeader = user && user.role === 1 ? '1' : '0'
  // Build payload matching backend expectations: include data_id and allowed fields
  const payload = Object.assign({}, editForm.value)
  // backend expects `data_id` (not `DataID`)
  if (payload.DataID) {
    payload.data_id = payload.DataID
    delete payload.DataID
  }
  // Only keep allowed update fields plus data_id
  const allowed = ['data_id','ShipID','DateTime_u','Lat','Long_u','SeaTemp','WaveHeight','WavePeriod','SurgeDirection','SurgeHeight']
  Object.keys(payload).forEach(k => { if (!allowed.includes(k)) delete payload[k] })
  try {
    const r = await updateData(payload, { 'User-Role': roleHeader, 'User-ID': user?.user_id || 0 })
    console.log('update response', r.data)
    if (r.data && r.data.success) {
      ElMessage.success('更新成功')
      editDialogVisible.value = false
      fetchDataList()
    } else ElMessage.error(r.data?.error || '更新失败')
  } catch (e) { ElMessage.error(e.message || '更新请求失败') }
}
</script>

<style scoped></style>
