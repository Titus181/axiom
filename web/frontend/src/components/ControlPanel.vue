<template>
  <section class="control-panel panel">
    <h2>模擬參數設定</h2>
    <form id="sim-form" @submit.prevent>
      <div class="form-group">
        <label for="game-type">選擇博弈模組</label>
        <select id="game-type" v-model="gameType" name="game_type" :disabled="isLoading">
          <option value="peek_baccarat">看牌百家樂 (Peek Baccarat)</option>
          <option value="dice" disabled>骰子點數模擬 (規劃中)</option>
          <option value="slots" disabled>老虎機模擬 (規劃中)</option>
        </select>
      </div>

      <div class="form-group">
        <label for="rounds">模擬局數 (Rounds)</label>
        <select id="rounds" v-model.number="rounds" name="rounds" :disabled="isLoading">
          <option value="10000">10,000 局 (極速)</option>
          <option value="100000">100,000 局 (推薦)</option>
          <option value="1000000">1,000,000 局 (大數驗證)</option>
          <option value="5000000">5,000,000 局 (精確收斂)</option>
          <option value="10000000">10,000,000 局 (1000 萬)</option>
          <option value="50000000">5,000,000 局 (5000 萬)</option>
          <option value="100000000">100,000,000 局 (1 億)</option>
          <option value="500000000">500,000,000 局 (5 億)</option>
          <option value="1000000000">1,000,000,000 局 (10 億)</option>
        </select>
      </div>

      <div class="form-group">
        <label>主注投注方向</label>
        <div class="radio-group">
          <label class="radio-label">
            <input 
              type="radio" 
              name="bet_type" 
              :value="1" 
              v-model.number="betType"
              :disabled="isLoading"
            >
            <span class="radio-button">閒家 (Player)</span>
          </label>
          <label class="radio-label">
            <input 
              type="radio" 
              name="bet_type" 
              :value="0" 
              v-model.number="betType"
              :disabled="isLoading"
            >
            <span class="radio-button">莊家 (Banker)</span>
          </label>
        </div>
      </div>

      <div class="form-group">
        <label for="seed">隨機數種子 (RNG Seed)</label>
        <input 
          type="number" 
          id="seed" 
          v-model.number="seed" 
          name="seed" 
          min="1"
          :disabled="isLoading"
        >
      </div>

      <button 
        type="button" 
        id="btn-run" 
        class="btn btn-primary" 
        :disabled="isLoading"
        @click="handleRun"
      >
        <span class="btn-text">{{ isLoading ? '模擬計算中...' : '開始數值模擬' }}</span>
        <div class="spinner" id="sim-spinner" v-if="isLoading"></div>
      </button>

      <div id="timing-info" class="timing-info" v-if="timing.show" style="display: flex;">
        <div class="timing-row">
          <span class="timing-label">開始時間:</span>
          <span id="time-start" class="timing-val">{{ timing.start }}</span>
        </div>
        <div class="timing-row">
          <span class="timing-label">結束時間:</span>
          <span id="time-end" class="timing-val">{{ timing.end }}</span>
        </div>
        <div class="timing-row total-time">
          <span class="timing-label">累計耗時:</span>
          <span id="time-elapsed" class="timing-val">{{ timing.elapsed }}</span>
        </div>
      </div>
    </form>
  </section>
</template>

<script setup lang="ts">
import { ref } from 'vue';

interface Timing {
  start: string;
  end: string;
  elapsed: string;
  show: boolean;
}

const props = defineProps<{
  isLoading: boolean;
  timing: Timing;
}>();

const emit = defineEmits<{
  (e: 'run', params: { game_type: string; rounds: number; bet_type: number; seed: number }): void;
}>();

const gameType = ref('peek_baccarat');
const rounds = ref(100000);
const betType = ref(1); // 1 = Player, 0 = Banker
const seed = ref(42);

const handleRun = () => {
  emit('run', {
    game_type: gameType.value,
    rounds: rounds.value,
    bet_type: betType.value,
    seed: seed.value
  });
};
</script>

<style scoped>
/* 樣式主要承襲全局 style.css，此處維持元件結構 */
</style>
