
from flask import jsonify
from flask import Flask,request
from flaskext.mysql import MySQL
from flask_cors import CORS, cross_origin
 
mysql = MySQL()
app = Flask(__name__)
CORS(app)

app.config['MYSQL_DATABASE_USER'] = 'root'
app.config['MYSQL_DATABASE_PASSWORD'] = '37'
app.config['MYSQL_DATABASE_DB'] = 'mysql'
app.config['MYSQL_DATABASE_HOST'] = 'localhost'
mysql.init_app(app)

@app.route("/realtime")
def real():
    try:
	cursor = mysql.connect().cursor()
	cursor.execute("SELECT * from LAST;")
	data = cursor.fetchall()
	dic_ret = {"samples":[{"ip":i[1],"pps":i[2], "ts":i[3]} for i in data]}


    except Exception as e:
        return str(e)

    return jsonify(dic_ret)


@app.route("/flowhistory")
def fhistory():
    try:
	flow = request.args.get('ip')
	ts = request.args.get('ts')
	cursor = mysql.connect().cursor()
	cursor.execute("SELECT * from HISTORY where FLOW='"+flow+"' AND TIMESTAMP > '"+ts+"';")
	data = cursor.fetchall()
	dic_ret = {"samples":[{"ip":i[1],"pps":i[2], "ts":i[3]} for i in data]}

    except Exception as e:
        return str(e)

    return jsonify(dic_ret)



@app.route("/history")
def history():
    try:
	cursor = mysql.connect().cursor()
	cursor.execute("SELECT COUNT(FLOW),FLOW FROM HISTORY GROUP BY FLOW ORDER BY COUNT(FLOW) DESC;")
	data = cursor.fetchall()
	dic_ret={}
	dic_ret["ip_list"] = [i[1] for i in data]

    except Exception as e:
        return str(e)

    return jsonify(dic_ret)

@app.route("/historyCounter")
def historyCounter():
    try:
	cursor = mysql.connect().cursor()
	cursor.execute("SELECT COUNT(FLOW),FLOW FROM HISTORY GROUP BY FLOW ORDER BY COUNT(FLOW) DESC;")
	data = cursor.fetchall()
	dic_ret={}
	dic_ret["ip_list"] = [{"ip":i[1],"counter":i[0]} for i in data]

    except Exception as e:
        return str(e)

    return jsonify(dic_ret)



if __name__ == "__main__":
    app.run(threaded=True)


