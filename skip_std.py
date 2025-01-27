import lldb
import os
import time
import asyncio
async def run_shell_command(command):
    """异步运行 shell 命令"""
    proc = await asyncio.create_subprocess_shell(
        command,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE
    )
    stdout, stderr = await proc.communicate()
    if stdout:
        print(f"[stdout]: {stdout.decode().strip()}")
    if stderr:
        print(f"[stderr]: {stderr.decode().strip()}")
def skip_in_std(debugger, command, result, internal_dict):
    # 获取当前线程和栈帧
    process = debugger.GetSelectedTarget().GetProcess()
    thread = process.GetSelectedThread()
    frame = thread.GetFrameAtIndex(0)

    func_name = frame.GetFunctionName()

    # 如果函数名以 'std::' 开头，则执行 step-out
    if func_name and func_name.startswith("std::"):
        debugger.HandleCommand("thread step-out")
        # time.sleep(0.25)
        # print("step-in again!")
        # asyncio.run(run_shell_command("keydown F11"))

        # os.system("keydown F11")
def __lldb_init_module(debugger, internal_dict):
    # 在 import 时，可以选择自动加一个别名命令
    debugger.HandleCommand('command script add -f skip_std.skip_in_std skip_in_std')
    print("Loaded skip_in_std command.")