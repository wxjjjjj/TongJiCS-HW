"""
数据库连接模块
"""
import mysql.connector
from config import Config

def get_db_connection():
    """
    获取数据库连接
    """
    try:
        conn = mysql.connector.connect(
            host=Config.MYSQL_HOST,
            user=Config.MYSQL_USER,
            password=Config.MYSQL_PASSWORD,
            database=Config.MYSQL_DATABASE,
            charset='utf8mb4',
            collation='utf8mb4_unicode_ci'
        )
        return conn
    except mysql.connector.Error as e:
        print(f"数据库连接错误: {e}")
        raise