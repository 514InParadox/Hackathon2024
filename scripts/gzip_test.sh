#!/bin/bash

# 确保提供了输入文件
if [ $# -eq 0 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

# 输入文件
input_file=$1

# 检查文件是否存在
if [ ! -f "$input_file" ]; then
    echo "Error: File '$input_file' not found!"
    exit 1
fi

# 输出文件大小函数
function human_readable_size {
    size=$(stat -c%s "$1")
    echo $(numfmt --to=iec --suffix=B $size)
}

# 压缩测试函数
function test_gzip {
    local level=$1
    local tmp_file="test_$level.gz"
    echo "Testing gzip compression level $level..."

    # 记录压缩开始时间
    start_time=$(date +%s%N)
    
    # 执行压缩
    gzip -c -"$level" "$input_file" > "$tmp_file"
    
    # 记录压缩结束时间
    end_time=$(date +%s%N)
    
    # 计算压缩时间（秒）
    elapsed_time=$(( ($end_time - $start_time) / 1000000 ))
    
    # 输出结果
    compressed_size=$(human_readable_size "$tmp_file")
    echo "Compression level: $level, Time: ${elapsed_time} ms, Size: $compressed_size"

    # 删除临时文件
    rm "$tmp_file"
}

# 循环测试不同的压缩级别
for level in {6..6}
do
    test_gzip $level
done

