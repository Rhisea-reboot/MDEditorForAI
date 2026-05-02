#!/usr/bin/env python3
"""
Skeleton Extractor - Python AST-based code skeletonization
Called by Qt application via QProcess for Python files
"""

import ast
import sys
import json


def extract_py_skeleton(source: str) -> str:
    """
    Extract skeleton from Python source using AST
    Returns: skeleton code with signatures and docstrings only
    """
    try:
        tree = ast.parse(source)
    except SyntaxError:
        # 如果解析失败，返回原始内容
        return source
    
    out = []
    
    for node in ast.iter_child_nodes(tree):
        # 处理类定义
        if isinstance(node, ast.ClassDef):
            bases = []
            for base in node.bases:
                if isinstance(base, ast.Name):
                    bases.append(base.id)
                elif isinstance(base, ast.Attribute):
                    bases.append(f"{base.value.id}.{base.attr}" if isinstance(base.value, ast.Name) else "...")
            
            base_str = f"({', '.join(bases)})" if bases else ""
            out.append(f"class {node.name}{base_str}:")
            
            # 提取类文档
            class_doc = ast.get_docstring(node)
            if class_doc:
                summary = class_doc.split('\n')[0][:80]
                out.append(f'    """{summary}..."""')
            
            # 处理类内成员
            for item in node.body:
                if isinstance(item, (ast.FunctionDef, ast.AsyncFunctionDef)):
                    # 提取函数签名
                    args_list = []
                    # 普通参数
                    for a in item.args.args:
                        if a.annotation and isinstance(a.annotation, ast.Name):
                            args_list.append(f"{a.arg}: {a.annotation.id}")
                        else:
                            args_list.append(a.arg)
                    # 默认参数
                    defaults_start = len(item.args.args) - len(item.args.defaults)
                    for i, default in enumerate(item.args.defaults):
                        idx = defaults_start + i
                        if isinstance(default, ast.Constant):
                            args_list[idx] = f"{args_list[idx]}={repr(default.value)[:30]}"
                    
                    args_str = ", ".join(args_list)
                    
                    # 返回类型
                    return_str = ""
                    if item.returns and isinstance(item.returns, ast.Name):
                        return_str = f" -> {item.returns.id}"
                    
                    func_line = f"    def {item.name}({args_str}){return_str}:"
                    out.append(func_line)
                    
                    # 提取 docstring
                    doc = ast.get_docstring(item)
                    if doc:
                        summary = doc.split('\n')[0][:80]
                        out.append(f"        # {summary}...")
                
                elif isinstance(item, ast.AnnAssign):
                    # 类型注解的属性
                    if isinstance(item.target, ast.Name):
                        attr_name = item.target.id
                        out.append(f"    {attr_name}: ...  # 类型注解属性")
                
                elif isinstance(item, ast.Assign):
                    # 类级常量（全大写）
                    for target in item.targets:
                        if isinstance(target, ast.Name) and target.id.isupper():
                            out.append(f"    # const: {target.id} = ...")
            
            out.append("")
            
        # 处理模块级函数
        elif isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
            args_list = []
            for a in node.args.args:
                if a.annotation and isinstance(a.annotation, ast.Name):
                    args_list.append(f"{a.arg}: {a.annotation.id}")
                else:
                    args_list.append(a.arg)
            
            defaults_start = len(node.args.args) - len(node.args.defaults)
            for i, default in enumerate(node.args.defaults):
                idx = defaults_start + i
                if isinstance(default, ast.Constant):
                    args_list[idx] = f"{args_list[idx]}={repr(default.value)[:30]}"
            
            args_str = ", ".join(args_list)
            
            return_str = ""
            if node.returns and isinstance(node.returns, ast.Name):
                return_str = f" -> {node.returns.id}"
            
            func_line = f"def {node.name}({args_str}){return_str}:"
            out.append(func_line)
            
            doc = ast.get_docstring(node)
            if doc:
                summary = doc.split('\n')[0][:80]
                out.append(f"    # {summary}...")
            
            out.append("")
        
        # 保留模块级常量（全大写）
        elif isinstance(node, ast.Assign):
            for target in node.targets:
                if isinstance(target, ast.Name) and target.id.isupper():
                    out.append(f"# const: {target.id} = ...")
        
        # 保留 Import 作为注释（只保留第一行）
        elif isinstance(node, ast.Import) and not any(isinstance(n, ast.Import) for n in list(ast.iter_child_nodes(tree))[:list(ast.iter_child_nodes(tree)).index(node)]):
            # 只输出一个 import 注释
            if not any("# import ..." in line for line in out):
                out.append("# import ... (imports omitted)")
    
    return '\n'.join(out)


if __name__ == "__main__":
    source = sys.stdin.read()
    result = extract_py_skeleton(source)
    print(result, end='')
