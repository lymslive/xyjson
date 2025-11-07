#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file generate_datasets.py
@author xyjson project
@date 2025-11-07
@brief 生成性能测试数据集脚本

此脚本用于生成不同规模的 JSON 文件，用于 xyjson 性能测试。
运行此脚本后会生成以下文件：
- datasets/small.json (约400B)
- datasets/medium.json (约30KB)
- datasets/large.json (约2MB)
"""

import json
import os

def generate_small():
    """生成小文件 - 简单嵌套结构"""
    data = {
        "status": "success",
        "data": {
            "user": {
                "id": 1,
                "name": "John Doe",
                "email": "john@example.com",
                "active": True
            },
            "metadata": {
                "version": "1.0",
                "created": "2025-01-01"
            }
        }
    }
    with open('datasets/small.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    print("Generated datasets/small.json")

def generate_medium():
    """生成中等文件 - 50个员工记录"""
    departments = ["Engineering", "Marketing", "Sales", "HR", "Finance"]
    projects = [f"Project-{chr(65+i)}" for i in range(20)]

    data = {
        "company": "Tech Corp International",
        "metrics": {
            "total_employees": 50,
            "avg_salary": 85000,
            "departments": 5,
            "founded": 2010,
            "location": "San Francisco, CA"
        },
        "employees": []
    }

    for i in range(1, 51):
        employee = {
            "id": i,
            "employee_id": f"EMP-{i:04d}",
            "name": f"Employee {i}",
            "first_name": f"FirstName{i}",
            "last_name": f"LastName{i}",
            "email": f"employee{i}@techcorp.com",
            "age": 25 + (i % 30),
            "department": departments[i % 5],
            "title": ["Software Engineer", "Senior Developer", "Tech Lead", "Manager", "Director"][i % 5],
            "salary": 50000 + (i * 1000),
            "hire_date": f"20{15 + (i % 8)}-{((i % 12) + 1):02d}-{((i % 28) + 1):02d}",
            "skills": [f"Programming Language {j}, Framework {j}, Tool {j}" for j in range(1, (i % 8) + 2)],
            "projects": [projects[j % 20] for j in range((i % 5) + 1)],
            "performance_rating": (i % 5) + 1,
            "manager_id": 1 if i > 10 else None,
            "team_size": (i % 10) + 1,
            "remote": i % 3 == 0,
            "active": True,
            "benefits": ["Health Insurance", "Dental", "Vision", "401k", "Stock Options"]
        }
        data["employees"].append(employee)

    with open('datasets/medium.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    print("Generated datasets/medium.json")

def generate_large():
    """生成大文件 - 1000个员工记录"""
    departments = ["Engineering", "Marketing", "Sales", "HR", "Finance", "Operations", "Legal", "R&D", "Support"]
    projects = [f"Project-{chr(65+i)}{chr(65+j)}" for i in range(20) for j in range(10)]

    data = {
        "company": "Large Corp International Ltd",
        "metrics": {
            "total_employees": 1000,
            "avg_salary": 82000,
            "departments": 9,
            "total_projects": 200,
            "offices": ["New York", "London", "Tokyo", "Berlin", "Paris", "Sydney"],
            "founded": 2005,
            "revenue": "$2.5B",
            "stock_symbol": "LCORP"
        },
        "employees": []
    }

    for i in range(1, 1001):
        employee = {
            "id": i,
            "employee_id": f"EMP-{i:05d}",
            "name": f"Employee {i}",
            "first_name": f"FirstName{i:03d}",
            "last_name": f"LastName{i:03d}",
            "email": f"employee{i}@largecorp.com",
            "alternate_email": f"{i}.lastname@largecorp.com",
            "age": 22 + (i % 45),
            "department": departments[i % 9],
            "title": ["Junior Engineer", "Engineer", "Senior Engineer", "Tech Lead", "Engineering Manager", "Director", "VP", "Senior VP", "CTO"][i % 9],
            "salary": 45000 + (i * 100) + ((i % 10) * 5000),
            "bonus": ((i * 100) % 20000),
            "hire_date": f"20{10 + (i % 14)}-{((i % 12) + 1):02d}-{((i % 28) + 1):02d}",
            "termination_date": None if i % 12 != 0 else f"20{15 + (i % 8)}-{((i % 12) + 1):02d}-{((i % 28) + 1):02d}",
            "skills": [f"Skill-{chr(65+j)}-{i%100}" for j in range(1, (i % 12) + 3)],
            "projects": [projects[j % 200] for j in range((i % 8) + 2)],
            "certifications": [f"Cert-{k}-{i%50}" for k in range((i % 3) + 1)],
            "performance_rating": (i % 5) + 1,
            "manager_id": 1 if i <= 50 else (i // 10) * 10 if (i // 10) * 10 < i else None,
            "team_size": (i % 15) + 1,
            "office_location": ["New York", "London", "Tokyo", "Berlin", "Paris", "Sydney"][i % 6],
            "remote": i % 4 == 0,
            "work_visa": i % 20 == 0,
            "languages": ["English"] + ([f"Language-{i%10}"] if i % 3 == 0 else []),
            "education": {
                "degree": ["Bachelor", "Master", "PhD", "MBA"][i % 4],
                "field": f"Field-{i%50}",
                "university": f"University-{i%100}",
                "graduation_year": 2000 + (i % 24)
            },
            "emergency_contact": {
                "name": f"Emergency-{i}",
                "relationship": ["Spouse", "Parent", "Sibling", "Friend"][i % 4],
                "phone": f"+1-555-{i:04d}"
            },
            "active": i % 10 != 0,
            "benefits": {
                "health_insurance": True,
                "dental": True,
                "vision": i % 3 == 0,
                "retirement_401k": True,
                "stock_options": i % 2 == 0,
                "paid_time_off": 15 + (i % 15),
                "parental_leave": i % 5 == 0
            }
        }
        data["employees"].append(employee)

    with open('datasets/large.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    print("Generated datasets/large.json")

def main():
    """主函数"""
    # 确保 datasets 目录存在
    os.makedirs('datasets', exist_ok=True)

    print("Generating performance test datasets...")
    print("=" * 50)

    generate_small()
    generate_medium()
    generate_large()

    print("=" * 50)
    print("All datasets generated successfully!")

    # 显示文件大小
    for filename in ['datasets/small.json', 'datasets/medium.json', 'datasets/large.json']:
        size = os.path.getsize(filename)
        print(f"{filename}: {size:,} bytes ({size/1024:.1f} KB)")

if __name__ == '__main__':
    main()
