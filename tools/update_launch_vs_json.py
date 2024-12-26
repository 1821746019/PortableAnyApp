import json
import sys
import argparse
from pathlib import Path

def update_or_create_launch_vs_json(file_path, project_target, exe_path, args=None, current_dir=None):
    try:
        # 读取文件内容
        if Path(file_path).is_file():
            with open(file_path, 'r', encoding='utf-8') as file:
                data = json.load(file)
        else:
            # 如果文件不存在，初始化一个新的结构
            data = {
                "version": "0.2.1",
                "defaults": {},
                "configurations": []
            }

        # 检查文件结构
        if 'configurations' not in data or not isinstance(data['configurations'], list):
            print("Error: Invalid format in launch.vs.json")
            return

        # 查找匹配的配置
        existing_config = None
        for config in data['configurations']:
            if config.get('projectTarget') == project_target:
                existing_config = config
                break

        if existing_config:
            # 更新已有配置
            existing_config['exe'] = exe_path
            if args is not None:
                existing_config['args'] = args
            else:
                existing_config['args'] = []
            if current_dir is not None:
                existing_config['currentDir'] = current_dir
            print(f"Updated existing configuration for projectTarget: {project_target}")
        else:
            # 创建新的配置
            new_config = {
                "type": "dll",
                "exe": exe_path,
                "project": "CMakeLists.txt",
                "projectTarget": project_target,
                "name": project_target
            }
            if args is not None:
                new_config['args'] = args
            else:
                new_config['args'] = []
            if current_dir is not None:
                new_config['currentDir'] = current_dir
            data['configurations'].append(new_config)
            print(f"Created new configuration for projectTarget: {project_target}")

        # 写回文件
        with open(file_path, 'w', encoding='utf-8') as file:
            json.dump(data, file, indent=2, ensure_ascii=False)

        print("Success: launch.vs.json updated successfully.")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # 设置参数解析器
    parser = argparse.ArgumentParser(description="Update or create a configuration in launch.vs.json.")
    parser.add_argument("file_path", help="Path to the launch.vs.json file.")
    parser.add_argument("project_target", help="The project target to update or create.")
    parser.add_argument("exe_path", help="The path to the executable.")
    parser.add_argument("-a", "--args", nargs="*", help="Arguments to pass to the executable.")
    parser.add_argument("-c", "--currentDir", help="Current directory for the executable.")

    args = parser.parse_args()

    # 执行更新或创建
    update_or_create_launch_vs_json(
        args.file_path,
        args.project_target,
        args.exe_path,
        args.args,
        args.currentDir
    )
