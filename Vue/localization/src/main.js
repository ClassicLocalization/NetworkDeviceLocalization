import { createApp } from 'vue'
import App from './App.vue'
import VueSocketio from 'vue-socket.io';


createApp(App).mount('#app')
this.use(VueSocketio, 'http://localhost:4000')
