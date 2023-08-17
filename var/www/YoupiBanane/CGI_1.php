<?php
$method = $_SERVER['REQUEST_METHOD'];
if ($method == "POST") {
  // echo "Status: 200 OK\r\n";
  // echo "Content-Type: text/html; charset=utf-8\r\n\r\n";
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
  // 이미지 저장 디렉토리 설정
  $uploadDir = "../upload/";

  // STDIN에서 이미지 바이너리 데이터 읽기
  while (!feof(STDIN)) {
    $inputData .= fread(STDIN, 1024); // 1024 바이트씩 읽음
}

  $errorLog = fopen("image.jpeg", "a"); // 에러 로그 파일 열기 (없을 경우 생성)
  if ($errorLog) {
      // 에러 메시지를 로그 파일에 기록
      fwrite($errorLog, $imageData);
      fclose($errorLog); // 파일 닫기
  } else {
      echo "에러 로그 파일 열기 실패";
  }
  // 이미지 이름 생성 (임의의 이름 또는 원본 파일 이름)
  $imageName = time() . "_" . rand(1000, 9999) . ".jpg"; // 예시: 타임스탬프_랜덤숫자.jpg

  // 이미지 파일 경로
  $imagePath = $uploadDir . $imageName;

  // 이미지 데이터를 파일로 저장
  // if (file_put_contents($imagePath, $imageData) !== false) {
  //     echo "이미지가 업로드되었습니다.";
  // } else {
  //     echo "이미지 업로드 중 오류가 발생했습니다.";
  // }
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
