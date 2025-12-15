# GitHub Pages 自动部署设置指南

本指南说明如何启用 docs/ 目录下 .md 文件的自动 GitHub Pages 部署。

## 可用方案

### 方案1：Jekyll (推荐)
- **文件**: `.github/workflows/docs.yml`
- **优点**: GitHub 原生支持，稳定可靠
- **启用**: 重命名为 `docs-jekyll.yml` 或删除其他方案文件

### 方案2：MkDocs (功能丰富)
- **文件**: `.github/workflows/mkdocs.yml`
- **优点**: 更好的主题支持，搜索功能
- **启用**: 重命名为 `docs.yml` 或删除其他方案文件

### 方案3：Pandoc (最灵活)
- **文件**: `.github/workflows/pandoc.yml`
- **优点**: 完全控制输出格式，自定义性强
- **启用**: 重命名为 `docs.yml` 或删除其他方案文件

## 启用步骤

### 1. 选择方案并重命名工作流

```bash
# 选择 Jekyll 方案（推荐）
mv .github/workflows/docs.yml .github/workflows/docs-jekyll.yml

# 或选择 MkDocs 方案
mv .github/workflows/mkdocs.yml .github/workflows/docs.yml

# 或选择 Pandoc 方案
mv .github/workflows/pandoc.yml .github/workflows/docs.yml
```

### 2. 启用 GitHub Pages

1. 进入 GitHub 仓库设置页面
2. 在左侧菜单中找到 "Pages"
3. 在 "Source" 中选择 "GitHub Actions"
4. 保存设置

### 3. 推送代码并测试

```bash
git add .
git commit -m "feat: add GitHub Pages documentation deployment"
git push
```

### 4. 访问文档

部署完成后，文档将在以下地址可用：
- `https://lymslive.github.io/xyjson/`

## 工作流触发条件

- 推送到 main 分支且 docs/ 目录有更改
- 手动触发 (`workflow_dispatch`)

## 方案对比

| 方案 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| Jekyll | 原生支持，稳定 | 主题相对简单 | 基本文档需求 |
| MkDocs | 功能丰富，搜索 | 需要额外配置 | 复杂文档站点 |
| Pandoc | 完全可控 | 配置较复杂 | 特殊格式需求 |

## 故障排除

### 构建失败
1. 检查 Actions 页面错误信息
2. 确认只有 `docs.yml` 工作流文件存在
3. 验证 docs/ 目录中的文件格式

### 部署失败
1. 确认仓库启用了 GitHub Pages
2. 检查权限设置（需要 contents 和 pages 写权限）
3. 验证分支保护规则

### 样式问题
1. 检查 CSS 文件路径
2. 验证 markdown 语法
3. 查看浏览器控制台错误

## 自定义配置

### Jekyll 自定义
编辑 `_config.yml` 文件：
- 修改主题和样式
- 添加插件
- 自定义导航

### MkDocs 自定义
编辑 `mkdocs.yml` 文件：
- 更改主题（Material for MkDocs）
- 添加扩展功能
- 自定义导航结构

### Pandoc 自定义
修改 workflow 中的 pandoc 命令：
- 更改 CSS 样式
- 添加转换选项
- 自定义 HTML 模板

## 维护建议

1. **定期检查**：确保文档链接正常
2. **版本兼容**：关注依赖更新
3. **备份文档**：重要的文档变更要备份
4. **测试预览**：重要更新前本地预览