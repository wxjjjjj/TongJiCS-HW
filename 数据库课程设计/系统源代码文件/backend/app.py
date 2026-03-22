"""
flask主程序
"""
from flask import Flask
from flask_cors import CORS
from routes.user_routes import user_bp
from routes.data_routes import data_bp
from routes.record_routes import record_bp

app = Flask(__name__)
CORS(app)  # 允许跨域

# 注册蓝图（模块化路由）
app.register_blueprint(user_bp, url_prefix='/api/user')
app.register_blueprint(data_bp, url_prefix='/api/data')
app.register_blueprint(record_bp, url_prefix='/api/record')

@app.route('/')
def home():
    return "船舶轨迹查询及海洋环境分析平台后端已启动！"

if __name__ == '__main__':
    app.run(debug=True, port=5000)