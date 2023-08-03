<?php
// API URL 및 요청 파라미터 설정
$API = "http://www.kma.go.kr/weather/forecast/mid-term-rss3.jsp";
$values = array('stnId' => '108'); // 서울 지역 (stnId 108)
$params = http_build_query($values);
$url = $API . "?" . $params;
// 환경변수에 METHOD가 있으면, METHOD 값을 읽어와서 method 변수에 저장
$method = $_SERVER['REQUEST_METHOD'];
if ($method == "GET") {
  // API 호출하여 데이터 가져오기
  $data = file_get_contents($url);
  // XML 데이터 파싱
  $root = new SimpleXMLElement($data);
  echo "<html>";
  echo "<head>";
  echo "<style>";
  echo "table {";
  echo "    border-collapse: collapse;";
  echo "    width: 80%;";
  echo "    margin: 0 auto;";
  echo "    text-align: center;";
  echo "}";
  echo "th, td {";
  echo "    border: 1px solid #dddddd;";
  echo "    padding: 8px;";
  echo "}";
  echo "th {";
  echo "    background-color: #f2f2f2;";
  echo "}";
  echo "</style>";
  echo "</head>";
  echo "<body>";
  // 날씨 정보 출력
  echo "<h2>서울 중기 날씨 예보</h2>";
  echo '<table>';
  echo '<tr><th>날짜</th><th>날씨</th><th>최저기온</th><th>최고기온</th></tr>';
  foreach ($root->xpath('.//data') as $data) {
      $date = $data->tmEf;
      $weather = $data->wf;
      $min_temp = $data->tmn;
      $max_temp = $data->tmx;
      echo "<tr><td>$date</td><td>$weather</td><td>$min_temp</td><td>$max_temp</td></tr>";
  }
  echo '</table>';
  echo "</body>";
  echo "</html>";
} elseif ($method == "POST") {
  $body = file_get_contents('php://input');
  // 바이너리 방식으로 작성된 body 이미지 데이터를 html로 변환
  echo "<html>";
  echo "<head>";
  echo "<style>";
  echo "table {";
  echo "    border-collapse: collapse;";
  echo "    width: 80%;";
  echo "    margin: 0 auto;";
  echo "    text-align: center;";
  echo "}";
  echo "th, td {";
  echo "    border: 1px solid #dddddd;";
  echo "    padding: 8px;";
  echo "}";
  echo "th {";
  echo "    background-color: #f2f2f2;";
  echo "}";
  echo "</style>";
  echo "</head>";
  echo "<body>";
  echo "<h2>이미지 업로드</h2>";
  echo "<img src='data:image/png;base64," . base64_encode($body) . "'>";
  echo "</body>";
  echo "</html>";
} else {
  echo "<html>";
  echo "<head>";
  echo "<style>";
  echo "table {";
  echo "    border-collapse: collapse;";
  echo "    width: 80%;";
  echo "    margin: 0 auto;";
  echo "    text-align: center;";
  echo "}";
  echo "th, td {";
  echo "    border: 1px solid #dddddd;";
  echo "    padding: 8px;";
  echo "}";
  echo "th {";
  echo "    background-color: #f2f2f2;";
  echo "}";
  echo "</style>";
  echo "</head>";
  echo "<body>";
  echo "<h2>지원하지 않는 메소드입니다.</h2>";
  echo "</body>";
  echo "</html>";
}
?>
