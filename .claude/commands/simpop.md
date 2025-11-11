---
description: "工具或文档等简单开发 TODO 需求"
argument-hint: "需求 ID"
---

任务 ID:
!`date +"%Y%m%d-%H%M%S"`

需求 ID:
$ARGUMENTS

需求内容（可从 `task_todo.md` 读取如下）：
=========================
!`perl script/todo.pl $ARGUMENTS`
=========================

请仔细分析如上需求内容，制定更详细的计划方案再开始实施，完成后记录日志与提交工作：
* 以任务 ID 为标题在 `tast_log.md` 末尾追加记录日志，参考已有格式
* 更新 `task_todo.md` 命令： perl script/todo.pl "需求ID" "任务ID"
* 提交 git ，提交消息的 scope 为需求ID($ARGUMENTS)

