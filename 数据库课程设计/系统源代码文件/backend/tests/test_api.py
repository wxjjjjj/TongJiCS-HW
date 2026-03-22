import requests

BASE = 'http://127.0.0.1:5000'

# Note: these tests are integration tests and assume the server is running.

def test_health():
    r = requests.get(BASE)
    assert r.status_code == 200

def test_ships():
    r = requests.get(BASE + '/api/data/ships')
    assert r.status_code == 200
    j = r.json()
    assert 'ships' in j

def test_import_csv():
    csv_path = 'tests/sample_import.csv'
    with open(csv_path, 'rb') as fh:
        files = {'file': ('sample_import.csv', fh, 'text/csv')}
        headers = {'User-Role': '1'}
        r = requests.post(BASE + '/api/data/import', files=files, headers=headers)
        assert r.status_code == 200
        j = r.json()
        assert 'imported' in j

if __name__ == '__main__':
    test_health()
    test_ships()
    test_import_csv()
