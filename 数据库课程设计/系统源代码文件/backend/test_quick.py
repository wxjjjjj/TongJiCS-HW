"""
后端功能的快速测试脚本
cd backend:
python test_quick.py
"""
import requests
import os

BASE = 'http://127.0.0.1:5000'

def test_health():
    print('health ->', requests.get(BASE).text)

def test_ships():
    r = requests.get(BASE + '/api/data/ships')
    print('ships ->', r.status_code, r.text)

def test_register_login():
    payload = {"email":"auto_test@example.com","password":"pwd123","name":"AutoTest","role":0}
    r = requests.post(BASE + '/api/user/register', json=payload)
    print('register ->', r.status_code, r.text)
    r = requests.post(BASE + '/api/user/login', json={"email":payload['email'],"password":payload['password']})
    print('login ->', r.status_code, r.text)

def test_import_csv():
    # 首先在脚本所在目录下查找（backend/tests/sample_import.csv），
    # 若不存在则回退到当前工作目录下的 tests/sample_import.csv
    script_dir = os.path.dirname(os.path.abspath(__file__))
    candidate1 = os.path.join(script_dir, 'tests', 'sample_import.csv')
    candidate2 = os.path.join('tests', 'sample_import.csv')
    if os.path.exists(candidate1):
        csv_path = candidate1
    elif os.path.exists(candidate2):
        csv_path = candidate2
    else:
        print('sample csv not found:', candidate1)
        return
    with open(csv_path, 'rb') as fh:
        files = {'file': ('sample_import.csv', fh, 'text/csv')}
        headers = {'User-Role': '1'}
        r = requests.post(BASE + '/api/data/import', files=files, headers=headers)
        print('import ->', r.status_code, r.text)

if __name__ == '__main__':
    test_health()
    test_ships()
    test_register_login()
    test_import_csv()
