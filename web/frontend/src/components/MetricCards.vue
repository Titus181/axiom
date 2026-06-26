<template>
  <section class="metrics-grid">
    <!-- Card 1: RTP -->
    <div class="metric-card card-rtp">
      <div class="card-header">
        <span class="card-title">玩家回報率 (RTP)</span>
        <span class="card-icon">⚡</span>
      </div>
      <div class="card-value" id="val-rtp">{{ displayRtp }}</div>
      <div class="card-subtitle">理論完美策略值: 98.80%</div>
    </div>

    <!-- Card 2: Win/Loss -->
    <div class="metric-card card-profit">
      <div class="card-header">
        <span class="card-title">累計淨損益 (Win/Loss)</span>
        <span class="card-icon">💰</span>
      </div>
      <div 
        class="card-value" 
        id="val-profit" 
        :style="{ color: profitColor }"
      >
        {{ displayProfit }}
      </div>
      <div class="card-subtitle" id="val-turnover">總投入: {{ displayTurnover }}</div>
    </div>

    <!-- Card 3: Volatility -->
    <div class="metric-card card-volatility">
      <div class="card-header">
        <span class="card-title">波動率 (Volatility)</span>
        <span class="card-icon">📈</span>
      </div>
      <div class="card-value" id="val-volatility">{{ displayVolatility }}</div>
      <div class="card-subtitle">數值穩定性指標</div>
    </div>

    <!-- Card 4: MDD -->
    <div class="metric-card card-mdd">
      <div class="card-header">
        <span class="card-title">最大回撤 (MDD)</span>
        <span class="card-icon">📉</span>
      </div>
      <div class="card-value" id="val-mdd">{{ displayMdd }}</div>
      <div class="card-subtitle">資產最大滑落幅度</div>
    </div>

    <!-- Card 5: Win Rates -->
    <div class="metric-card card-win-rates">
      <div class="card-header">
        <span class="card-title">對局勝率佔比</span>
        <span class="card-icon">📊</span>
      </div>
      <div class="card-value" id="val-win-rates" style="font-size: 1.15rem; font-family: inherit; line-height: 1.5; margin-top: 5px; font-weight: 500;">
        <div style="display: flex; justify-content: space-between;">
          <span>莊贏 (Banker):</span>
          <span id="val-banker-rate" style="color: #f59e0b; font-weight: 700;">{{ displayBankerRate }}</span>
        </div>
        <div style="display: flex; justify-content: space-between;">
          <span>閒贏 (Player):</span>
          <span id="val-player-rate" style="color: #3b82f6; font-weight: 700;">{{ displayPlayerRate }}</span>
        </div>
        <div style="display: flex; justify-content: space-between;">
          <span>和局 (Tie):</span>
          <span id="val-tie-rate" style="color: #10b981; font-weight: 700;">{{ displayTieRate }}</span>
        </div>
      </div>
      <div class="card-subtitle">實際模擬勝率分佈</div>
    </div>
  </section>
</template>

<script setup lang="ts">
import { computed } from 'vue';

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

const props = defineProps<{
  metrics: Metrics | null;
}>();

const formatCurrency = (val: number) => {
  return val.toLocaleString(undefined, {
    minimumFractionDigits: 2,
    maximumFractionDigits: 2
  });
};

const displayRtp = computed(() => {
  if (!props.metrics) return '--%';
  return (props.metrics.rtp * 100).toFixed(4) + '%';
});

const displayProfit = computed(() => {
  if (!props.metrics) return '$0.00';
  const profit = props.metrics.total_win_loss;
  return (profit >= 0 ? '+' : '') + '$' + formatCurrency(profit);
});

const profitColor = computed(() => {
  if (!props.metrics) return 'inherit';
  return props.metrics.total_win_loss >= 0 ? '#10b981' : '#ef4444';
});

const displayTurnover = computed(() => {
  if (!props.metrics) return '$0.00';
  return '$' + formatCurrency(props.metrics.total_turnover);
});

const displayVolatility = computed(() => {
  if (!props.metrics) return '0.0000';
  return props.metrics.volatility.toFixed(4);
});

const displayMdd = computed(() => {
  if (!props.metrics) return '$0.00';
  return '$' + formatCurrency(props.metrics.max_drawdown);
});

const displayBankerRate = computed(() => {
  if (!props.metrics) return '--%';
  return (props.metrics.banker_win_rate * 100).toFixed(2) + '%';
});

const displayPlayerRate = computed(() => {
  if (!props.metrics) return '--%';
  return (props.metrics.player_win_rate * 100).toFixed(2) + '%';
});

const displayTieRate = computed(() => {
  if (!props.metrics) return '--%';
  return (props.metrics.tie_rate * 100).toFixed(2) + '%';
});
</script>

<style scoped>
/* 樣式主要承襲全局 style.css，此處維持元件結構 */
</style>
