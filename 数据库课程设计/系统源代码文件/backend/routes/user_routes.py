"""
用户user_u路由/API接口
"""
from flask import Blueprint, request, jsonify
from models.database import get_db_connection
from werkzeug.security import generate_password_hash, check_password_hash

user_bp = Blueprint('user', __name__)

@user_bp.route('/login', methods=['POST'])
def login():
    data = request.json
    email = data.get('email')
    password = data.get('password')

    conn = get_db_connection()
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT UserID, Email, Password_u, Name_u, Role_u FROM User_u WHERE Email = %s", (email,))
    user = cursor.fetchone()
    cursor.close()
    conn.close()

    if not user:
        return jsonify({"error": "邮箱或密码错误"}), 401

    stored = user.get('Password_u')
    # 支持旧明文密码：如果 stored 看起来像明文且和输入相等，则允许并升级为哈希
    try:
        password_ok = False
        if stored and (stored.startswith('pbkdf2:') or stored.startswith('sha256$')):
            password_ok = check_password_hash(stored, password)
        else:
            # 旧记录可能是明文
            if password == stored:
                password_ok = True
                # 升级为哈希
                conn = get_db_connection()
                cur2 = conn.cursor()
                new_hash = generate_password_hash(password)
                cur2.execute("UPDATE User_u SET Password_u = %s WHERE UserID = %s", (new_hash, user['UserID']))
                conn.commit()
                cur2.close()
                conn.close()
        if not password_ok:
            return jsonify({"error": "邮箱或密码错误"}), 401
    except Exception:
        return jsonify({"error": "认证失败"}), 500

    # 不返回密码字段
    safe_user = {
        'user_id': user.get('UserID'),
        'email': user.get('Email'),
        'name': user.get('Name_u'),
        'role': user.get('Role_u')
    }
    return jsonify({"message": "登录成功", "user": safe_user})

@user_bp.route('/register', methods=['POST'])
def register():
    data = request.json
    email = data.get('email')
    password = data.get('password')
    name = data.get('name')
    role = data.get('role', 0)  # 默认普通用户

    conn = get_db_connection()
    cursor = conn.cursor()
    try:
        hashed = generate_password_hash(password)
        cursor.execute(
            "INSERT INTO User_u (Email, Password_u, Name_u, Role_u) VALUES (%s, %s, %s, %s)",
            (email, hashed, name, role)
        )
        conn.commit()
        return jsonify({"message": "注册成功"})
    except Exception as e:
        conn.rollback()
        return jsonify({"error": str(e)}), 400
    finally:
        cursor.close()
        conn.close()