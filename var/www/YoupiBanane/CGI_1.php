<?php
$method = $_SERVER['REQUEST_METHOD'];
if ($method == "POST") {
  echo "Status: 200 OK";
  echo "Content-Type: text/html; charset=utf-8\r\n\r\n";
  // echo "";
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
  echo "</html>\r\n\r\n";
} else {
  echo "Status: 501 INVALID_METHOD";
  echo "Content-Type: text/html; charset=utf-8\r\n\r\n";
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
  echo "</html>\r\n\r\n";
}
?>
