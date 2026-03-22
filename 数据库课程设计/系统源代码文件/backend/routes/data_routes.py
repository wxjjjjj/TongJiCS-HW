"""
data_u路由/API接口
"""
from flask import Blueprint, request, jsonify
from models.database import get_db_connection
import mysql.connector
from datetime import datetime
import traceback
import io, csv

data_bp = Blueprint('data', __name__)


def _validate_user_id(user_id):
    """验证提供的 user_id 是否存在于 User_u 表；存在返回 int，否则返回 None"""
    try:
        uid = int(user_id)
    except Exception:
        return None
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('SELECT UserID FROM User_u WHERE UserID = %s', (uid,))
        row = cur.fetchone()
        cur.close()
        conn.close()
        return uid if row is not None else None
    except Exception:
        return None

# 查询船舶轨迹数据
@data_bp.route('/vessel-tracks', methods=['GET'])
def get_vessel_tracks():
    """
    查询船舶轨迹数据
    参数: ship_id, start_time, end_time
    """
    try:
        ship_id = request.args.get('ship_id', type=int)
        start_time = request.args.get('start_time')
        end_time = request.args.get('end_time')
        
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        
        # 构建查询条件
        query = """
        SELECT ShipID, DateTime_u, Lat, Long_u, SeaTemp, WaveHeight, WavePeriod, SurgeDirection, SurgeHeight
        FROM Data_u 
        WHERE 1=1
        """
        params = []
        
        if ship_id:
            query += " AND ShipID = %s"
            params.append(ship_id)
        if start_time:
            query += " AND DateTime_u >= %s"
            params.append(start_time)
        if end_time:
            query += " AND DateTime_u <= %s"
            params.append(end_time)

        query += " ORDER BY DateTime_u ASC"
        
        cursor.execute(query, params)
        tracks = cursor.fetchall()
        
        # 格式化返回数据
        formatted_tracks = []
        for track in tracks:
            formatted_tracks.append({
                'ship_id': track['ShipID'],
                'datetime': track['DateTime_u'].strftime('%Y-%m-%d %H:%M:%S') if track['DateTime_u'] else None,
                'latitude': float(track['Lat']),
                'longitude': float(track['Long_u']),
                'sea_temperature': float(track['SeaTemp']) if track['SeaTemp'] is not None else None,
                'wave_height': float(track['WaveHeight']),
                'wave_period': float(track['WavePeriod']),
                'surge_direction': track['SurgeDirection'],
                'surge_height': float(track['SurgeHeight'])
            })
        
        cursor.close()
        conn.close()
        
        return jsonify({
            "success": True,
            "data": formatted_tracks,
            "count": len(formatted_tracks)
        })
        
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

# 查询海洋环境数据
@data_bp.route('/ocean-environment', methods=['GET'])
def get_ocean_environment():
    """
    查询特定区域的海洋环境数据
    参数: min_lat, max_lat, min_lng, max_lng, start_time, end_time
    """
    try:
        min_lat = request.args.get('min_lat', type=float)
        max_lat = request.args.get('max_lat', type=float)
        min_lng = request.args.get('min_lng', type=float)
        max_lng = request.args.get('max_lng', type=float)
        start_time = request.args.get('start_time')
        end_time = request.args.get('end_time')
        
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        
        query = """
        SELECT DateTime_u, Lat, Long_u, SeaTemp, WaveHeight, WavePeriod, 
               SurgeDirection, SurgeHeight, ShipID
        FROM Data_u 
        WHERE 1=1
        """
        params = []
        
        if min_lat is not None and max_lat is not None:
            query += " AND Lat BETWEEN %s AND %s"
            params.extend([min_lat, max_lat])
        if min_lng is not None and max_lng is not None:
            query += " AND Long_u BETWEEN %s AND %s"
            params.extend([min_lng, max_lng])
        if start_time:
            query += " AND DateTime_u >= %s"
            params.append(start_time)
        if end_time:
            query += " AND DateTime_u <= %s"
            params.append(end_time)

        query += " ORDER BY DateTime_u DESC LIMIT 1000"  # 限制返回数量
        
        cursor.execute(query, params)
        environment_data = cursor.fetchall()
        
        # 格式化数据
        formatted_data = []
        for data in environment_data:
            formatted_data.append({
                'datetime': data['DateTime_u'].strftime('%Y-%m-%d %H:%M:%S') if data['DateTime_u'] else None,
                'latitude': float(data['Lat']),
                'longitude': float(data['Long_u']),
                'sea_temperature': float(data['SeaTemp']) if data['SeaTemp'] is not None else None,
                'wave_height': float(data['WaveHeight']),
                'wave_period': float(data['WavePeriod']),
                'surge_direction': data['SurgeDirection'],
                'surge_height': float(data['SurgeHeight']),
                'ship_id': data['ShipID']
            })
        
        cursor.close()
        conn.close()
        
        return jsonify({
            "success": True,
            "data": formatted_data,
            "count": len(formatted_data)
        })
        
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

# 管理员添加数据
@data_bp.route('/add', methods=['POST'])
def add_data():
    """
    管理员添加船舶和环境数据
    """
    try:
        # 检查用户权限（这里需要从session或token获取用户角色）
        # 简化处理，实际项目中需要完整的认证机制
        user_role = request.headers.get('User-Role')
        if user_role != '1':  # 1表示管理员
            return jsonify({"success": False, "error": "权限不足"}), 403
        
        data = request.json
        
        # 验证必需字段
        required_fields = ['ship_id', 'datetime', 'lat', 'long', 'sea_temp', 
                          'wave_height', 'wave_period', 'surge_direction', 'surge_height']
        for field in required_fields:
            if field not in data:
                return jsonify({"success": False, "error": f"缺少必需字段: {field}"}), 400
        
        # 验证涌浪方向
        if data['surge_direction'] not in ['E', 'S', 'W', 'N']:
            return jsonify({"success": False, "error": "涌浪方向必须是 E, S, W, N 之一"}), 400
        
        conn = get_db_connection()
        cursor = conn.cursor()
        
        insert_query = """
        INSERT INTO Data_u (ShipID, DateTime_u, Lat, Long_u, SeaTemp, WaveHeight, 
                 WavePeriod, SurgeDirection, SurgeHeight)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
        """
        
        cursor.execute(insert_query, (
            data['ship_id'],
            data['datetime'],
            data['lat'],
            data['long'],
            data['sea_temp'],
            data['wave_height'],
            data['wave_period'],
            data['surge_direction'],
            data['surge_height']
        ))
        
        conn.commit()
        new_data_id = cursor.lastrowid
        
        cursor.close()
        conn.close()
        
        # 记录该操作到 Record（尝试记录 user id 如果提供）
        try:
            conn2 = get_db_connection()
            cur2 = conn2.cursor()
            user_id = request.headers.get('User-ID') or 0
            valid_uid = _validate_user_id(user_id)
            cur2.execute("INSERT INTO Record (OprateType, DateTime_u, UserID, DataID) VALUES (%s, %s, %s, %s)",
                         ('INSERT', datetime.now(), valid_uid, int(new_data_id)))
            conn2.commit()
            cur2.close()
            conn2.close()
        except Exception as e:
            print(f"记录操作到 Record 失败 (INSERT single add): {e}")

        return jsonify({
            "success": True,
            "message": "数据添加成功",
            "data_id": new_data_id
        })
        
    except mysql.connector.Error as e:
        return jsonify({"success": False, "error": f"数据库错误: {str(e)}"}), 500
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

# 获取船舶列表
@data_bp.route('/ships', methods=['GET'])
def get_ships():
    """
    获取所有船舶ID列表
    """
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        cursor.execute("SELECT DISTINCT ShipID FROM Data_u ORDER BY ShipID")
        ships = [row[0] for row in cursor.fetchall()]
        
        cursor.close()
        conn.close()
        
        return jsonify({
            "success": True,
            "ships": ships
        })
        
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500


# 批量导入 CSV（管理员）
@data_bp.route('/import', methods=['POST'])
def import_csv():
    """
    管理员批量导入 CSV 文件（multipart/form-data, field name: file）
    CSV 列名应包含: ship_id, datetime, lat, long, sea_temp, wave_height, wave_period, surge_direction, surge_height
    返回: { success, imported, failed: [{row, reason}], error }
    """
    user_role = request.headers.get('User-Role')
    if user_role != '1':
        return jsonify({"success": False, "error": "权限不足"}), 403

    if 'file' not in request.files:
        return jsonify({"success": False, "error": "缺少文件参数 'file'"}), 400

    f = request.files['file']
    try:
        text = f.stream.read().decode('utf-8')
    except Exception:
        return jsonify({"success": False, "error": "无法读取上传文件，需 utf-8 编码"}), 400

    reader = csv.DictReader(io.StringIO(text))
    # 将 reader 转为列表以便统计总行数并在遇到单行错误时继续处理后续行
    rows = list(reader)
    required_fields = ['ship_id', 'datetime', 'lat', 'long', 'sea_temp', 'wave_height', 'wave_period', 'surge_direction', 'surge_height']

    conn = get_db_connection()
    cursor = conn.cursor()
    imported = 0
    failed = []
    user_id = request.headers.get('User-ID') or 0

    insert_query = """
    INSERT INTO Data_u (ShipID, DateTime_u, Lat, Long_u, SeaTemp, WaveHeight, WavePeriod, SurgeDirection, SurgeHeight)
    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
    """

    conn.start_transaction()
    for idx, row in enumerate(rows, start=1):
            # 验证字段
            missing = [f for f in required_fields if f not in row or row[f] == '']
            if missing:
                failed.append({"row": idx, "reason": f"缺少字段: {','.join(missing)}"})
                continue

            if row['surge_direction'] not in ['E', 'S', 'W', 'N']:
                failed.append({"row": idx, "reason": "涌浪方向必须是 E/S/W/N 之一"})
                continue

            try:
                params = (
                    int(row['ship_id']),
                    row['datetime'],
                    float(row['lat']),
                    float(row['long']),
                    float(row['sea_temp']) if row['sea_temp'] != '' else None,
                    float(row['wave_height']) if row['wave_height'] != '' else None,
                    float(row['wave_period']) if row['wave_period'] != '' else None,
                    row['surge_direction'],
                    float(row['surge_height']) if row['surge_height'] != '' else None,
                )
                cursor.execute(insert_query, params)
                new_id = cursor.lastrowid
                # 写入操作记录（记录失败不应中断导入）
                try:
                    valid_uid = _validate_user_id(user_id)
                    cursor.execute("INSERT INTO Record (OprateType, DateTime_u, UserID, DataID) VALUES (%s, %s, %s, %s)",
                                   ('IMPORT', datetime.now(), valid_uid, new_id))
                except Exception as rec_ex:
                    # 打印日志以便排查，但继续导入其他行
                    print(f"记录操作到 Record 失败 (IMPORT) row={idx}: {rec_ex}")
                imported += 1
            except Exception as ex:
                failed.append({"row": idx, "reason": str(ex)})
                # continue importing other rows
    # 尝试提交事务；若提交失败则回滚并报告错误（此时已汇总导入和失败行）
    try:
        conn.commit()
    except Exception as e:
        conn.rollback()
        cursor.close()
        conn.close()
        return jsonify({"success": False, "error": f"提交事务失败: {str(e)}", "imported": imported, "failed": failed}), 500

    cursor.close()
    conn.close()
    total_rows = len(rows)
    return jsonify({"success": True, "total_rows": total_rows, "imported": imported, "failed": failed})


# 管理员：列出所有数据（分页）
@data_bp.route('/list', methods=['GET'])
def list_data():
    try:
        user_role = request.headers.get('User-Role')
        if user_role != '1':
            return jsonify({"success": False, "error": "权限不足"}), 403

        page = request.args.get('page', 1, type=int)
        per_page = request.args.get('per_page', 50, type=int)

        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)

        count_q = "SELECT COUNT(*) as total FROM Data_u"
        cursor.execute(count_q)
        total = cursor.fetchone()['total']

        q = "SELECT * FROM Data_u ORDER BY DateTime_u DESC LIMIT %s OFFSET %s"
        cursor.execute(q, (per_page, (page - 1) * per_page))
        rows = cursor.fetchall()

        # 返回全部列（按 Data_u 表常用字段格式化日期和数值）
        formatted = []
        for r in rows:
            formatted.append({
                'DataID': r.get('DataID'),
                'ShipID': r.get('ShipID'),
                'DateTime_u': r['DateTime_u'].strftime('%Y-%m-%d %H:%M:%S') if r.get('DateTime_u') else None,
                'Lat': float(r['Lat']) if r.get('Lat') is not None else None,
                'Long_u': float(r['Long_u']) if r.get('Long_u') is not None else None,
                'SeaTemp': float(r['SeaTemp']) if r.get('SeaTemp') is not None else None,
                'WaveHeight': float(r['WaveHeight']) if r.get('WaveHeight') is not None else None,
                'WavePeriod': float(r['WavePeriod']) if r.get('WavePeriod') is not None else None,
                'SurgeDirection': r.get('SurgeDirection'),
                'SurgeHeight': float(r['SurgeHeight']) if r.get('SurgeHeight') is not None else None
            })

        cursor.close()
        conn.close()

        return jsonify({
            'success': True,
            'data': formatted,
            'pagination': {
                'page': page,
                'per_page': per_page,
                'total': total,
                'pages': (total + per_page - 1) // per_page
            }
        })
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500


# 管理员：删除指定 DataID
@data_bp.route('/delete', methods=['POST'])
def delete_data():
    try:
        user_role = request.headers.get('User-Role')
        if user_role != '1':
            return jsonify({"success": False, "error": "权限不足"}), 403

        data = request.json or {}
        data_id = data.get('data_id')
        if not data_id:
            return jsonify({"success": False, "error": "缺少 data_id"}), 400

        conn = get_db_connection()
        cursor = conn.cursor()

        # 可先检查是否存在
        cursor.execute('SELECT DataID FROM Data_u WHERE DataID = %s', (data_id,))
        if cursor.fetchone() is None:
            cursor.close()
            conn.close()
            return jsonify({"success": False, "error": "数据不存在"}), 404

        # 先在 Record 中记录删除操作（父记录仍存在时插入不会触发外键错误）
        record_written = False
        user_id = request.headers.get('User-ID') or 0
        try:
            conn2 = get_db_connection()
            cur2 = conn2.cursor()
            valid_uid = _validate_user_id(user_id)
            cur2.execute("INSERT INTO Record (OprateType, DateTime_u, UserID, DataID) VALUES (%s, %s, %s, %s)",
                         ('DELETE', datetime.now(), valid_uid, int(data_id)))
            conn2.commit()
            cur2.close()
            conn2.close()
            record_written = True
        except Exception as e:
            # 如果记录写入失败，打印错误但继续删除主记录
            print(f"在删除前记录到 Record 失败 (DELETE) data_id={data_id}: {e}")

        # 执行删除主记录
        try:
            cursor.execute('DELETE FROM Data_u WHERE DataID = %s', (data_id,))
            conn.commit()
        except Exception as del_ex:
            # 若删除失败，尝试回滚并报告错误
            conn.rollback()
            cursor.close()
            conn.close()
            return jsonify({"success": False, "error": f"删除失败: {del_ex}"}), 500

        cursor.close()
        conn.close()
        return jsonify({"success": True, "message": "删除成功", "data_id": data_id, "record_written": record_written})
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500


# 管理员：更新指定 DataID 的字段
@data_bp.route('/update', methods=['POST'])
def update_data():
    try:
        user_role = request.headers.get('User-Role')
        if user_role != '1':
            return jsonify({"success": False, "error": "权限不足"}), 403

        payload = request.json or {}
        data_id = payload.get('data_id')
        if not data_id:
            return jsonify({"success": False, "error": "缺少 data_id"}), 400

        # 允许更新的字段列表（根据 Data_u 表）
        allowed = ['ShipID', 'DateTime_u', 'Lat', 'Long_u', 'SeaTemp', 'WaveHeight', 'WavePeriod', 'SurgeDirection', 'SurgeHeight']
        updates = []
        params = []
        for k in allowed:
            if k in payload:
                updates.append(f"{k} = %s")
                params.append(payload[k])

        if not updates:
            return jsonify({"success": False, "error": "没有要更新的字段"}), 400

        params.append(data_id)

        conn = get_db_connection()
        cursor = conn.cursor()

        sql = f"UPDATE Data_u SET {', '.join(updates)} WHERE DataID = %s"
        # debug: print constructed SQL and params
        try:
            print('DEBUG update payload:', payload)
            print('DEBUG sql:', sql)
            print('DEBUG params:', params)
            cursor.execute(sql, tuple(params))
        except Exception as exec_err:
            print('Update execute failed:', exec_err)
            traceback.print_exc()
            raise
        conn.commit()

        # 记录操作（使用独立连接，保证与更新事务隔离）
        record_written = False
        user_id = request.headers.get('User-ID') or 0
        try:
            conn2 = get_db_connection()
            cur2 = conn2.cursor()
            valid_uid = _validate_user_id(user_id)
            cur2.execute("INSERT INTO Record (OprateType, DateTime_u, UserID, DataID) VALUES (%s, %s, %s, %s)",
                         ('UPDATE', datetime.now(), valid_uid, int(data_id)))
            conn2.commit()
            cur2.close()
            conn2.close()
            record_written = True
        except Exception as e:
            print(f"记录操作到 Record 失败 (UPDATE) data_id={data_id}: {e}")

        cursor.close()
        conn.close()

        return jsonify({"success": True, "message": "更新成功", "data_id": data_id, "record_written": record_written})
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500