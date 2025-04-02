# 目标路径（修改为您的目录路径）
$targetPath = "./fruits"

# 获取所有的.png文件
$pngFiles = Get-ChildItem -Path $targetPath -Recurse -File -Filter *.png

# 统计文件数量
$pngCount = $pngFiles.Count

# 打印结果
Write-Host "该目录下共有 $pngCount 个 .png 文件。"
