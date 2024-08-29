import subprocess
import time
import os

def run_sperf(command, interval=1, duration=5):
    # 构建strace命令，将输出保存到临时文件
    output_file = "strace_output.txt"
    start_time = time.time()

    for count in range(duration):
        strace_command = ['strace', '-c', '-f', '-o', output_file] + command

        # 启动strace命令
        process = subprocess.Popen(strace_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        # 等待一秒，确保有数据输出
        time.sleep(interval)

        # 终止strace收集信息
        process.terminate()

        # 检查并解析strace的输出
        if os.path.exists(output_file) and os.path.getsize(output_file) > 0:
            with open(output_file, "r") as f:
                result = f.read()
                syscall_stats = []
                collecting = False
                for line in result.splitlines():
                    if '------' in line:
                        collecting = not collecting
                        continue
                    if collecting:
                        parts = line.split()
                        if len(parts) >= 5:  # 确保每行有足够的字段
                            syscall_name = parts[-1].lower()
                            syscall_time = parts[0]
                            syscall_stats.append((syscall_name, syscall_time))

            # 输出当前时间段的系统调用统计信息
            elapsed_time = int(time.time() - start_time)
            print(f"Time: {elapsed_time}s")
            if syscall_stats:
                for i, syscall in enumerate(syscall_stats[:5]):
                    syscall_name, syscall_time = syscall
                    # 输出格式为：syscall_name(%time)
                    print(f"{syscall_name}({syscall_time}%)")
            else:
                print("No syscall data captured.")
            print("=" * 25)
            
            # 清空输出文件
            open(output_file, 'w').close()

    # 输出结束符号
    print("8^")
    print("\\e")

    # 删除临时文件
    if os.path.exists(output_file):
        os.remove(output_file)

if __name__ == "__main__":
    command = ['ls', '-l']  # 你可以在这里指定要执行的命令
    run_sperf(command, interval=1, duration=5)
