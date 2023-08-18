import cgitb; cgitb.enable()
import os
import urllib.request
import urllib.parse
import xml.etree.ElementTree as ET

#환경변수에 METHOD가 있으면, METHOD 값을 읽어와서 method 변수에 저장
method = os.environ.get('REQUEST_METHOD')
if method == "GET":
  print("Status: 200 OK")
  print("Content-Type: text/html; charset=utf-8\r\n\r\n")
  # print()
  # API URL 및 요청 파라미터 설정
  API = "http://www.kma.go.kr/weather/forecast/mid-term-rss3.jsp"
  values = {'stnId': '108'}  # 서울 지역 (stnId 108)
  params = urllib.parse.urlencode(values)
  url = API + "?" + params
  # API 호출하여 데이터 가져오기
  response = urllib.request.urlopen(url)
  data = response.read()
  # XML 데이터 파싱
  root = ET.fromstring(data)
  print("<html>")
  print("<head>")
  print("<style>")
  print("table {")
  print("    border-collapse: collapse;")
  print("    width: 80%;")
  print("    margin: 0 auto;")
  print("    text-align: center;")
  print("}")
  print("th, td {")
  print("    border: 1px solid #dddddd;")
  print("    padding: 8px;")
  print("}")
  print("th {")
  print("    background-color: #f2f2f2;")
  print("}")
  print("</style>")
  print("</head>")
  print("<body>")
  # 날씨 정보 출력
  print("<h2>서울 중기 날씨 예보</h2>")
  print('<table>')
  print('<tr><th>날짜</th><th>날씨</th><th>최저기온</th><th>최고기온</th></tr>')
  for data in root.findall('.//data'):
      date = data.find('tmEf').text
      weather = data.find('wf').text
      min_temp = data.find('tmn').text
      max_temp = data.find('tmx').text
      print(f'<tr><td>{date}</td><td>{weather}</td><td>{min_temp}</td><td>{max_temp}</td></tr>')
  print('</table>')
  print("</body>\r\n\r\n")
else:
  print("Status: 501 INVALID_METHOD")
  print("Content-Type: text/html; charset=utf-8\r\n\r\n")
  print("<html>")
  print("<head>")
  print("<style>")
  print("table {")
  print("    border-collapse: collapse;")
  print("    width: 80%;")
  print("    margin: 0 auto;")
  print("    text-align: center;")
  print("}")
  print("th, td {")
  print("    border: 1px solid #dddddd;")
  print("    padding: 8px;")
  print("}")
  print("th {")
  print("    background-color: #f2f2f2;")
  print("}")
  print("</style>")
  print("</head>")
  print("<body>")
  print("<h2>지원하지 않는 메소드입니다.</h2>")
  print("</body>")
print("</html>\r\n\r\n")
