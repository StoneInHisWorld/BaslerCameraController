Measure-Command {
    # 目标路径
    $targetPath = "./fruits"

    # 获取所有文件（递归）
    $files = Get-ChildItem -Path $targetPath -Recurse -File

    # 多线程删除
    $files | ForEach-Object -Parallel {
        Remove-Item $_.FullName -Force -ErrorAction SilentlyContinue
    } -ThrottleLimit 10
}
