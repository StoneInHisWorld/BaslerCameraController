# 定义源目录和目标目录
$sourcePath = "./fruits"
$destinationPath = "C:\Users\hpdqd\Documents\Tencent Files\1195290789\FileRecv\研究生期间研究\Ultra_DIMPLE\linear_fruits\QuickDraw\camdis6.0cm\train"

# 确保目标目录存在（如果不存在则创建）
if (!(Test-Path -Path $destinationPath)) {
    New-Item -ItemType Directory -Path $destinationPath
}

# 获取所有文件（递归）
$files = Get-ChildItem -Path $sourcePath -Recurse -File
$totalFiles = $files.Count

# 初始化计数器
$completedFiles = 0

# 移动文件并显示进度条
foreach ($file in $files) {
    try {
        # 移动文件
        Move-Item -Path $file.FullName -Destination $destinationPath -Force

        # 更新完成计数
        $completedFiles++

        # 显示进度条
        Write-Progress -Activity "移动文件中..." -Status "$completedFiles / $totalFiles" `
            -PercentComplete (($completedFiles / $totalFiles) * 100)
    } catch {
        Write-Host "移动失败: $($file.FullName)" -ForegroundColor Red
    }
}

Write-Host "所有文件已成功移动！"
