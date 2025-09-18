# -STM32-

基于HAL库FREERtos的STM32的六轴空心杯无人机

该分支为default默认分支readme

git的分支框架如下：
/
/
/
readme分支(默认分支)

main分支(把更新的且已经完成了的工程提交到这里来，比如说李芳今天完成了给整个工程打上了光流的驱动)

function achieve分支(实现了某模块的功能，比如说李芳昨天打完了驱动，今天实现了光流定点/邓煜实现了遥控器发送数据到串口。并且还要提交到main分支)

debug分支(发现其他分支里的工程有bug，提交到这里，debug结束后重新提交回main/function achieve)

unfinished分支(写一半的代码提交到这里)

注意事项：
/
/
/
先git add.把改动的文件放到暂存区
每次提交都要备注一下(git commit -m "我实现了角度环，角速度环的双环pid")  ///把 已经 git add 到暂存区 的文件，提交到 本地仓库（.git），并记录一条提交说明。这一步并没有提交到github库///
然后git push origin main
要确认你的本地库分支和远程库(GitHub库)分支是不是一样的
