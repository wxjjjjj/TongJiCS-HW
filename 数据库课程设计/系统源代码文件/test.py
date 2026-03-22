from flask import Flask #导入Flask类

app = Flask(__name__) #实例化

@app.route('/') #默认根地址，只要运行起来，打开网址就会触发的route路由
def index():
    return "ship"

if __name__=='__main__':
    app.run(debug=True)

