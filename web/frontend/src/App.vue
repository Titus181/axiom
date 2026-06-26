<template>
  <div class="app-container">
    <!-- Header -->
    <header class="app-header">
      <div class="logo">
        <span class="logo-accent">Axiom</span>
        <span class="logo-sub">Platform v2 (Vue 3)</span>
      </div>
      <div class="status-indicator">
        <span class="dot active"></span>
        <span class="status-text">C++ 模擬核心已連線</span>
      </div>
    </header>

    <!-- Main Layout -->
    <main class="app-main">
      <!-- Left Panel: Control Form -->
      <ControlPanel 
        :isLoading="isLoading"
        :timing="timing"
        @run="runSimulation"
      />

      <!-- Right Content: Dashboard -->
      <div class="dashboard-content">
        <!-- Metrics Cards -->
        <MetricCards :metrics="metrics" />

        <!-- Tabs Panel for Charts and Report -->
        <section class="tab-panel panel">
          <div class="tabs">
            <button 
              class="tab-btn" 
              :class="{ active: activeTab === 'tab-charts' }" 
              @click="activeTab = 'tab-charts'"
            >
              即時圖表分析
            </button>
            <button 
              class="tab-btn" 
              :class="{ active: activeTab === 'tab-report' }" 
              @click="activeTab = 'tab-report'"
            >
              數值規格報告 (Markdown)
            </button>
          </div>

          <!-- Tab Content 1: Charts -->
          <div 
            class="tab-content" 
            :class="{ active: activeTab === 'tab-charts' }"
            id="tab-charts"
          >
            <ConvergenceCharts :trends="trends" />
          </div>

          <!-- Tab Content 2: Report -->
          <div 
            class="tab-content" 
            :class="{ active: activeTab === 'tab-report' }"
            id="tab-report"
          >
            <ReportViewer :reportUrl="reportUrl" />
          </div>
        </section>
      </div>
    </main>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import ControlPanel from './components/ControlPanel.vue';
import MetricCards from './components/MetricCards.vue';
import ConvergenceCharts from './components/ConvergenceCharts.vue';
import ReportViewer from './components/ReportViewer.vue';

interface Timing {
  start: string;
  end: string;
  elapsed: string;
  show: boolean;
}

interface Metrics {
  rtp: number;
  total_win_loss: number;
  total_turnover: number;
  volatility: number;
  max_drawdown: number;
  banker_win_rate: number;
  player_win_rate: number;
  tie_rate: number;
}

interface Trends {
  rounds: number[];
  rtp: number[];
  balance: number[];
}

const isLoading = ref(false);
const activeTab = ref('tab-charts');
const metrics = ref<Metrics | null>(null);
const trends = ref<Trends | null>(null);
const reportUrl = ref<string | null>(null);

const timing = ref<Timing>({
  start: '--',
  end: '--',
  elapsed: '0.00 秒',
  show: false
});

const formatTime = (date: Date): string => {
  const pad = (n: number) => String(n).padStart(2, '0');
  const padMs = (n: number) => String(n).padStart(3, '0');
  return `${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())} ` +
         `${pad(date.getHours())}:${pad(date.getMinutes())}:${pad(date.getSeconds())}.${padMs(date.getMilliseconds())}`;
};

const formatDuration = (seconds: number): string => {
  if (seconds < 60) {
    return seconds.toFixed(2) + " 秒";
  }
  const days = Math.floor(seconds / 86400);
  seconds %= 86400;
  const hours = Math.floor(seconds / 3600);
  seconds %= 3600;
  const minutes = Math.floor(seconds / 60);
  const secs = seconds % 60;
  
  let res = "";
  if (days > 0) res += days + " 天 ";
  if (hours > 0 || days > 0) res += hours + " 小時 ";
  if (minutes > 0 || hours > 0 || days > 0) res += minutes + " 分 ";
  res += secs.toFixed(2) + " 秒";
  return res;
};

let elapsedTimer: ReturnType<typeof setInterval> | null = null;

const runSimulation = async (params: { game_type: string; rounds: number; bet_type: number; seed: number }) => {
  isLoading.value = true;
  timing.value.show = true;
  
  const startTime = new Date();
  timing.value.start = formatTime(startTime);
  timing.value.end = '運算中...';
  timing.value.elapsed = '0.00 秒';

  if (elapsedTimer) clearInterval(elapsedTimer);
  elapsedTimer = setInterval(() => {
    const now = new Date();
    const diff = (now.getTime() - startTime.getTime()) / 1000;
    timing.value.elapsed = formatDuration(diff);
  }, 50);

  try {
    const response = await fetch('/api/simulate', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(params)
    });

    if (!response.ok) {
      const errorData = await response.json();
      throw new Error(errorData.detail || '模擬執行失敗');
    }

    const data = await response.json();

    if (elapsedTimer) clearInterval(elapsedTimer);
    const endTime = new Date();
    timing.value.end = formatTime(endTime);
    const totalDiff = (endTime.getTime() - startTime.getTime()) / 1000;
    timing.value.elapsed = formatDuration(totalDiff);

    metrics.value = data.metrics;
    trends.value = data.trends;
    reportUrl.value = data.report_url;

  } catch (error: any) {
    if (elapsedTimer) clearInterval(elapsedTimer);
    timing.value.end = '執行失敗';
    alert(`執行模擬出錯: ${error.message}`);
  } finally {
    isLoading.value = false;
  }
};
</script>

<style scoped>
/* 樣式主要承襲全局 style.css，此處維持元件結構 */
</style>
