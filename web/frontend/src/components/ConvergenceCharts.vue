<template>
  <div class="chart-container">
    <div class="chart-wrapper" style="height: 380px; display: flex; flex-direction: column;">
      <h3>RTP 收斂曲線 (Monte Carlo)</h3>
      <canvas ref="rtpCanvas" style="max-height: 300px; flex-grow: 1;"></canvas>
    </div>
    <div class="chart-wrapper" style="height: 380px; display: flex; flex-direction: column;">
      <h3>累計淨收益曲線 (Asset Trend)</h3>
      <canvas ref="balanceCanvas" style="max-height: 300px; flex-grow: 1;"></canvas>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch, onUnmounted, nextTick } from 'vue';
import Chart from 'chart.js/auto';

interface Trends {
  rounds: number[];
  rtp: number[];
  balance: number[];
}

const props = defineProps<{
  trends: Trends | null;
}>();

const rtpCanvas = ref<HTMLCanvasElement | null>(null);
const balanceCanvas = ref<HTMLCanvasElement | null>(null);

let rtpChartInstance: Chart | null = null;
let balanceChartInstance: Chart | null = null;

const destroyCharts = () => {
  if (rtpChartInstance) {
    rtpChartInstance.destroy();
    rtpChartInstance = null;
  }
  if (balanceChartInstance) {
    balanceChartInstance.destroy();
    balanceChartInstance = null;
  }
};

const renderCharts = () => {
  destroyCharts();

  if (!props.trends || !rtpCanvas.value || !balanceCanvas.value) return;

  const commonOptions = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        labels: { color: "#8b9bb4" }
      }
    },
    scales: {
      x: {
        grid: { color: "rgba(255, 255, 255, 0.05)" },
        ticks: { color: "#8b9bb4" }
      },
      y: {
        grid: { color: "rgba(255, 255, 255, 0.05)" },
        ticks: { color: "#8b9bb4" }
      }
    }
  };

  // 1. RTP 收斂圖
  rtpChartInstance = new Chart(rtpCanvas.value, {
    type: "line",
    data: {
      labels: props.trends.rounds,
      datasets: [
        {
          label: "模擬 RTP (%)",
          data: props.trends.rtp.map(v => v * 100),
          borderColor: "#3b82f6",
          borderWidth: 2,
          pointRadius: 0,
          fill: false
        },
        {
          label: "理論期望 RTP (98.80%)",
          data: Array(props.trends.rounds.length).fill(98.80),
          borderColor: "#ef4444",
          borderDash: [5, 5],
          pointRadius: 0,
          fill: false
        }
      ]
    },
    options: commonOptions
  });

  // 2. 累計收益圖
  balanceChartInstance = new Chart(balanceCanvas.value, {
    type: "line",
    data: {
      labels: props.trends.rounds,
      datasets: [
        {
          label: "累計損益 ($)",
          data: props.trends.balance,
          borderColor: "#10b981",
          borderWidth: 2,
          pointRadius: 0,
          fill: true,
          backgroundColor: "rgba(16, 185, 129, 0.05)"
        }
      ]
    },
    options: commonOptions
  });
};

watch(() => props.trends, () => {
  nextTick(() => {
    renderCharts();
  });
}, { deep: true });

onUnmounted(() => {
  destroyCharts();
});
</script>

<style scoped>
/* 樣式主要承襲全局 style.css，此處維持元件結構 */
</style>
