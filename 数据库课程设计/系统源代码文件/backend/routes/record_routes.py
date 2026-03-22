"""
record路由/API接口
"""
from flask import Blueprint, request, jsonify
from models.database import get_db_connection
from datetime import datetime

record_bp = Blueprint('record', __name__)

# 记录用户操作
def log_operation(user_id, data_id, operation_type):
    """
    记录用户操作的辅助函数
    """
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        insert_query = """
        INSERT INTO Record (OprateType, DateTime_u, UserID, DataID)
        VALUES (%s, %s, %s, %s)
        """
        
        cursor.execute(insert_query, (
            operation_type,
            datetime.now(),
            user_id,
            data_id
        ))
        
        conn.commit()
        cursor.close()
        conn.close()
        return True
    except Exception as e:
        print(f"记录操作失败: {e}")
        return False

# 查询操作记录
@record_bp.route('/operations', methods=['GET'])
def get_operations():
    """
    查询操作记录（管理员功能）
    参数: user_id, operation_type, start_time, end_time
    """
    try:
        # 检查管理员权限
        user_role = request.headers.get('User-Role')
        if user_role != '1':  # 1表示管理员
            return jsonify({"success": False, "error": "权限不足"}), 403
        
        user_id = request.args.get('user_id', type=int)
        operation_type = request.args.get('operation_type')
        start_time = request.args.get('start_time')
        end_time = request.args.get('end_time')
        page = request.args.get('page', 1, type=int)
        per_page = request.args.get('per_page', 50, type=int)
        
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        
        # 构建查询
        query = """
        SELECT r.RecordID, r.OprateType, r.DateTime_u, r.UserID, r.DataID,
               u.Name_u as UserName, u.Email as UserEmail,
               d.ShipID, d.DateTime_u as DataTime
        FROM Record r
        LEFT JOIN User_u u ON r.UserID = u.UserID
        LEFT JOIN Data_u d ON r.DataID = d.DataID
        WHERE 1=1
        """
        params = []
        
        if user_id:
            query += " AND r.UserID = %s"
            params.append(user_id)
        if operation_type:
            query += " AND r.OprateType = %s"
            params.append(operation_type)
        if start_time:
            query += " AND r.DateTime_u >= %s"
            params.append(start_time)
        if end_time:
            query += " AND r.DateTime_u <= %s"
            params.append(end_time)
            
        # 计算总数
        count_query = "SELECT COUNT(*) as total FROM (" + query + ") as t"
        cursor.execute(count_query, params)
        total = cursor.fetchone()['total']
        
        # 添加分页
        query += " ORDER BY r.DateTime_u DESC LIMIT %s OFFSET %s"
        params.extend([per_page, (page - 1) * per_page])
        
        cursor.execute(query, params)
        operations = cursor.fetchall()
        
        # 格式化返回数据
        formatted_operations = []
        for op in operations:
            formatted_operations.append({
                'record_id': op['RecordID'],
                'operation_type': op['OprateType'],
                'datetime': op['DateTime_u'].strftime('%Y-%m-%d %H:%M:%S') if op['DateTime_u'] else None,
                'user_id': op['UserID'],
                'user_name': op['UserName'],
                'user_email': op['UserEmail'],
                'data_id': op['DataID'],
                'ship_id': op['ShipID'],
                'data_time': op['DataTime'].strftime('%Y-%m-%d %H:%M:%S') if op['DataTime'] else None
            })
        
        cursor.close()
        conn.close()
        
        return jsonify({
            "success": True,
            "data": formatted_operations,
            "pagination": {
                "page": page,
                "per_page": per_page,
                "total": total,
                "pages": (total + per_page - 1) // per_page
            }
        })
        
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

# 获取操作统计
@record_bp.route('/stats', methods=['GET'])
def get_operation_stats():
    """
    获取操作统计信息（管理员功能）
    """
    try:
        # 检查管理员权限
        user_role = request.headers.get('User-Role')
        if user_role != '1':
            return jsonify({"success": False, "error": "权限不足"}), 403
        
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        
        # 按操作类型统计
        cursor.execute("""
            SELECT OprateType, COUNT(*) as count 
            FROM Record 
            GROUP BY OprateType
        """)
        type_stats = cursor.fetchall()
        
        # 按用户统计
        cursor.execute("""
            SELECT u.Name_u as Name, u.Email as Email, COUNT(*) as operation_count
            FROM Record r
            JOIN User_u u ON r.UserID = u.UserID
            GROUP BY r.UserID
            ORDER BY operation_count DESC
            LIMIT 10
        """)
        user_stats = cursor.fetchall()
        
        # 最近活动
        cursor.execute("""
            SELECT COUNT(*) as recent_activity
            FROM Record 
            WHERE DateTime_u >= DATE_SUB(NOW(), INTERVAL 7 DAY)
        """)
        recent_activity = cursor.fetchone()['recent_activity']
        
        cursor.close()
        conn.close()
        
        return jsonify({
            "success": True,
            "stats": {
                "by_operation_type": type_stats,
                "top_users": user_stats,
                "recent_activity_7_days": recent_activity
            }
        })
        
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

# 清理旧记录
@record_bp.route('/cleanup', methods=['POST'])
def cleanup_old_records():
    """
    清理超过一定时间的操作记录（管理员功能）
    """
    try:
        # 检查管理员权限
        user_role = request.headers.get('User-Role')
        if user_role != '1':
            return jsonify({"success": False, "error": "权限不足"}), 403
        
        data = request.json
        days_to_keep = data.get('days_to_keep', 90)  # 默认保留90天
        
        conn = get_db_connection()
        cursor = conn.cursor()
        
        delete_query = "DELETE FROM Record WHERE DateTime_u < DATE_SUB(NOW(), INTERVAL %s DAY)"
        cursor.execute(delete_query, (days_to_keep,))
        
        deleted_count = cursor.rowcount
        conn.commit()
        
        cursor.close()
        conn.close()
        
        return jsonify({
            "success": True,
            "message": f"成功清理 {deleted_count} 条旧记录",
            "deleted_count": deleted_count
        })
        
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500