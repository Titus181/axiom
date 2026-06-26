<template>
  <div id="report-viewer" class="markdown-body">
    <div v-if="isLoading" class="empty-state">
      <p>報告載入中...</p>
      <div class="spinner" style="margin: 10px auto;"></div>
    </div>
    
    <div v-else-if="error" class="error-state">
      <p>載入報告出錯: {{ error }}</p>
    </div>

    <div v-else-if="renderedHtml" v-html="renderedHtml"></div>

    <div v-else class="empty-state">
      <p>尚未執行模擬。請在左側設定參數並點擊「開始數值模擬」以生成分析報告。</p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue';
import { marked } from 'marked';

const props = defineProps<{
  reportUrl: string | null;
}>();

const renderedHtml = ref<string>('');
const isLoading = ref<boolean>(false);
const error = ref<string | null>(null);

const fetchAndRenderReport = async (url: string) => {
  isLoading.value = true;
  error.value = null;
  renderedHtml.value = '';

  try {
    const response = await fetch(url);
    if (!response.ok) throw new Error("載入報告失敗");

    const data = await response.json();
    // 使用 marked 解析 markdown
    const html = await marked.parse(data.content || '');
    renderedHtml.value = html;
  } catch (err: any) {
    error.value = err.message || '未知錯誤';
  } finally {
    isLoading.value = false;
  }
};

watch(() => props.reportUrl, (newUrl) => {
  if (newUrl) {
    fetchAndRenderReport(newUrl);
  } else {
    renderedHtml.value = '';
    error.value = null;
  }
});
</script>

<style scoped>
/* 樣式主要承襲全局 style.css，此處維持元件結構 */
</style>
