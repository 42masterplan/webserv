<?php
$method = $_SERVER['REQUEST_METHOD'];
if ($method == "POST") {
  echo "Status: 200 OK";
  echo "Content-Type: text/html; charset=utf-8\r\n\r\n";
  // echo "";
  // $body = file_get_contents('php://input');
  // 바이너리 방식으로 작성된 body 이미지 데이터를 html로 변환
  // echo "<html>";
  // echo "<head>";
  // echo "<style>";
  // echo "table {";
  // echo "    border-collapse: collapse;";
  // echo "    width: 80%;";
  // echo "    margin: 0 auto;";
  // echo "    text-align: center;";
  // echo "}";
  // echo "th, td {";
  // echo "    border: 1px solid #dddddd;";
  // echo "    padding: 8px;";
  // echo "}";
  // echo "th {";
  // echo "    background-color: #f2f2f2;";
  // echo "}";
  // echo "</style>";
  // echo "</head>";
  // echo "<body>";
  if (isset($_FILES["image"])) {
    $uploadDir = "../upload/"; // 이미지를 저장할 디렉토리
    $uploadedFile = $uploadDir . basename($_FILES["image"]["name"]);
    
    // 파일 확장자 검사 (보다 엄격한 검증 필요)
    $imageFileType = strtolower(pathinfo($uploadedFile, PATHINFO_EXTENSION));
    if ($imageFileType == "jpg" || $imageFileType == "png" || $imageFileType == "jpeg" || $imageFileType == "gif") {
        if (move_uploaded_file($_FILES["image"]["tmp_name"], $uploadedFile)) {
            echo "파일이 업로드되었습니다.";
        } else {
            echo "파일 업로드 중 오류가 발생했습니다.";
        }
    } else {
        echo "올바른 이미지 파일 형식을 선택하세요.";
    }
  }
  // echo "</body>";
  // echo "</html>\r\n\r\n";
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
