import { defineStore } from 'pinia'

export const useUserStore = defineStore('user', {
  state: () => ({
    user: null
  }),
  getters: {
    isAdmin: (state) => !!(state.user && state.user.role === 1)
  },
  actions: {
    setUser(u) {
      this.user = u
      try { localStorage.setItem('user', JSON.stringify(u)) } catch (e) {}
    },
    clearUser() {
      this.user = null
      try { localStorage.removeItem('user') } catch (e) {}
    },
    restoreFromStorage() {
      try {
        const raw = localStorage.getItem('user')
        this.user = raw ? JSON.parse(raw) : null
      } catch (e) { this.user = null }
    }
  }
})
