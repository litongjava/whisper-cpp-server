# Client Code
# shell
## cUrl
```shell
curl --location --request POST 'http://192.168.3.9:8080/inference' \
--header 'User-Agent: Apifox/1.0.0 (https://apifox.com)' \
--header 'Accept: */*' \
--header 'Host: 192.168.3.9:8080' \
--header 'Connection: keep-alive' \
--header 'Content-Type: multipart/form-data; boundary=--------------------------671827497522367123871197' \
--form 'file=@"E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav"' \
--form 'temperature="0.2"' \
--form 'response-format="json"' \
--form 'audio_format="wav"'
```

## curl-windows
```
curl --location --request POST "http://192.168.3.9:8080/inference" ^
--header "User-Agent: Apifox/1.0.0 (https://apifox.com)" ^
--header "Accept: */*" ^
--header "Host: 192.168.3.9:8080" ^
--header "Connection: keep-alive" ^
--header "Content-Type: multipart/form-data; boundary=--------------------------671827497522367123871197" ^
--form "file=@\"E:\\\\code\\\\cpp\\\\cpp-study\\\\cpp-study-clion\\\\audio\\\\jfk.wav\"" ^
--form "temperature=\"0.2\"" ^
--form "response-format=\"json\"" ^
--form "audio_format=\"wav\""
```
## httpie
```
http --ignore-stdin --form --follow --timeout 3600 POST 'http://192.168.3.9:8080/inference' \
 'file'@E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav \
 'temperature'@undefined \
 'response-format'@undefined \
 'audio_format'@undefined \
 User-Agent:'Apifox/1.0.0 (https://apifox.com)' \
 Accept:'*/*' \
 Host:'192.168.3.9:8080' \
 Connection:'keep-alive' \
 Content-Type:'multipart/form-data; boundary=--------------------------671827497522367123871197'
```

## wget
```
# wget doesn't support file upload via form data, use curl -F \
wget --no-check-certificate --quiet \
   --method POST \
   --timeout=0 \
   --header 'User-Agent: Apifox/1.0.0 (https://apifox.com)' \
   --header 'Accept: */*' \
   --header 'Host: 192.168.3.9:8080' \
   --header 'Connection: keep-alive' \
   --header 'Content-Type: multipart/form-data; boundary=--------------------------671827497522367123871197' \
   --body-data '' \
    'http://192.168.3.9:8080/inference'
```
## power shell
```
$headers = New-Object "System.Collections.Generic.Dictionary[[String],[String]]"
$headers.Add("User-Agent", "Apifox/1.0.0 (https://apifox.com)")
$headers.Add("Accept", "*/*")
$headers.Add("Host", "192.168.3.9:8080")
$headers.Add("Connection", "keep-alive")
$headers.Add("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197")

$multipartContent = [System.Net.Http.MultipartFormDataContent]::new()
$multipartFile = 'E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav'
$FileStream = [System.IO.FileStream]::new($multipartFile, [System.IO.FileMode]::Open)
$fileHeader = [System.Net.Http.Headers.ContentDispositionHeaderValue]::new("form-data")
$fileHeader.Name = "file"
$fileHeader.FileName = "E:`\code`\cpp`\cpp-study`\cpp-study-clion`\audio`\jfk.wav"
$fileContent = [System.Net.Http.StreamContent]::new($FileStream)
$fileContent.Headers.ContentDisposition = $fileHeader
$multipartContent.Add($fileContent)

$stringHeader = [System.Net.Http.Headers.ContentDispositionHeaderValue]::new("form-data")
$stringHeader.Name = "temperature"
$stringContent = [System.Net.Http.StringContent]::new("0.2")
$stringContent.Headers.ContentDisposition = $stringHeader
$multipartContent.Add($stringContent)

$stringHeader = [System.Net.Http.Headers.ContentDispositionHeaderValue]::new("form-data")
$stringHeader.Name = "response-format"
$stringContent = [System.Net.Http.StringContent]::new("json")
$stringContent.Headers.ContentDisposition = $stringHeader
$multipartContent.Add($stringContent)

$stringHeader = [System.Net.Http.Headers.ContentDispositionHeaderValue]::new("form-data")
$stringHeader.Name = "audio_format"
$stringContent = [System.Net.Http.StringContent]::new("wav")
$stringContent.Headers.ContentDisposition = $stringHeader
$multipartContent.Add($stringContent)

$body = $multipartContent

$response = Invoke-RestMethod 'http://192.168.3.9:8080/inference' -Method 'POST' -Headers $headers -Body $body
$response | ConvertTo-Json
```
# JavaScript
## fetch
```
var myHeaders = new Headers();
myHeaders.append("User-Agent", "Apifox/1.0.0 (https://apifox.com)");
myHeaders.append("Accept", "*/*");
myHeaders.append("Host", "192.168.3.9:8080");
myHeaders.append("Connection", "keep-alive");
myHeaders.append("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197");

var formdata = new FormData();
formdata.append("file", fileInput.files[0], "E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav");
formdata.append("temperature", "0.2");
formdata.append("response-format", "json");
formdata.append("audio_format", "wav");

var requestOptions = {
   method: 'POST',
   headers: myHeaders,
   body: formdata,
   redirect: 'follow'
};

fetch("http://192.168.3.9:8080/inference", requestOptions)
   .then(response => response.text())
   .then(result => console.log(result))
   .catch(error => console.log('error', error));
```
## axios
```
var axios = require('axios');
var FormData = require('form-data');
var fs = require('fs');
var data = new FormData();
data.append('file', fs.createReadStream('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav'));
data.append('temperature', '0.2');
data.append('response-format', 'json');
data.append('audio_format', 'wav');

var config = {
   method: 'post',
   url: 'http://192.168.3.9:8080/inference',
   headers: { 
      'User-Agent': 'Apifox/1.0.0 (https://apifox.com)', 
      'Accept': '*/*', 
      'Host': '192.168.3.9:8080', 
      'Connection': 'keep-alive', 
      'Content-Type': 'multipart/form-data; boundary=--------------------------671827497522367123871197', 
      ...data.getHeaders()
   },
   data : data
};

axios(config)
.then(function (response) {
   console.log(JSON.stringify(response.data));
})
.catch(function (error) {
   console.log(error);
});
```
## jQuery
```
var form = new FormData();
form.append("file", fileInput.files[0], "E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav");
form.append("temperature", "0.2");
form.append("response-format", "json");
form.append("audio_format", "wav");

var settings = {
   "url": "http://192.168.3.9:8080/inference",
   "method": "POST",
   "timeout": 0,
   "headers": {
      "User-Agent": "Apifox/1.0.0 (https://apifox.com)",
      "Accept": "*/*",
      "Host": "192.168.3.9:8080",
      "Connection": "keep-alive",
      "Content-Type": "multipart/form-data; boundary=--------------------------671827497522367123871197"
   },
   "processData": false,
   "mimeType": "multipart/form-data",
   "contentType": false,
   "data": form
};

$.ajax(settings).done(function (response) {
   console.log(response);
});
```

## XHR
```
// WARNING: For POST requests, body is set to null by browsers.
var data = new FormData();
data.append("file", fileInput.files[0], "E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav");
 data.append("temperature", "0.2");
data.append("response-format", "json");
data.append("audio_format", "wav");

var xhr = new XMLHttpRequest();
xhr.withCredentials = true;

xhr.addEventListener("readystatechange", function() {
   if(this.readyState === 4) {
      console.log(this.responseText);
   }
});

xhr.open("POST", "http://192.168.3.9:8080/inference");
xhr.setRequestHeader("User-Agent", "Apifox/1.0.0 (https://apifox.com)");
xhr.setRequestHeader("Accept", "*/*");
xhr.setRequestHeader("Host", "192.168.3.9:8080");
xhr.setRequestHeader("Connection", "keep-alive");
xhr.setRequestHeader("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197");

xhr.send(data);
```

## Native
```
var http = require('follow-redirects').http;
var fs = require('fs');

var options = {
   'method': 'POST',
   'hostname': '192.168.3.9',
   'port': 8080,
   'path': '/inference',
   'headers': {
      'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
      'Accept': '*/*',
      'Host': '192.168.3.9:8080',
      'Connection': 'keep-alive',
      'Content-Type': 'multipart/form-data; boundary=--------------------------671827497522367123871197'
   },
   'maxRedirects': 20
};

var req = http.request(options, function (res) {
   var chunks = [];

   res.on("data", function (chunk) {
      chunks.push(chunk);
   });

   res.on("end", function (chunk) {
      var body = Buffer.concat(chunks);
      console.log(body.toString());
   });

   res.on("error", function (error) {
      console.error(error);
   });
});

var postData = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=\"E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav\"\r\nContent-Type: \"{Insert_File_Content_Type}\"\r\n\r\n" + fs.readFileSync('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav') + "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"temperature\"\r\n\r\n0.2\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"response-format\"\r\n\r\njson\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"audio_format\"\r\n\r\nwav\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";

req.setHeader('content-type', 'multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW');

req.write(postData);

req.end();
```

## Request
```
var request = require('request');
var fs = require('fs');
var options = {
   'method': 'POST',
   'url': 'http://192.168.3.9:8080/inference',
   'headers': {
      'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
      'Accept': '*/*',
      'Host': '192.168.3.9:8080',
      'Connection': 'keep-alive',
      'Content-Type': 'multipart/form-data; boundary=--------------------------671827497522367123871197'
   },
   formData: {
      'file': {
         'value': fs.createReadStream('E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav'),
         'options': {
            'filename': 'E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav',
            'contentType': null
         }
      },
      'temperature': '0.2',
      'response-format': 'json',
      'audio_format': 'wav'
   }
};
request(options, function (error, response) {
   if (error) throw new Error(error);
   console.log(response.body);
});
```
## UniRest
```
var unirest = require('unirest');
var req = unirest('POST', 'http://192.168.3.9:8080/inference')
   .headers({
      'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
      'Accept': '*/*',
      'Host': '192.168.3.9:8080',
      'Connection': 'keep-alive',
      'Content-Type': 'multipart/form-data; boundary=--------------------------671827497522367123871197'
   })
   .attach('file', 'E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav')
   .field('temperature', '0.2')
   .field('response-format', 'json')
   .field('audio_format', 'wav')
   .end(function (res) { 
      if (res.error) throw new Error(res.error); 
      console.log(res.raw_body);
   });
```

# Java
## Unirest
```
Unirest.setTimeouts(0, 0);
HttpResponse<String> response = Unirest.post("http://192.168.3.9:8080/inference")
   .header("User-Agent", "Apifox/1.0.0 (https://apifox.com)")
   .header("Accept", "*/*")
   .header("Host", "192.168.3.9:8080")
   .header("Connection", "keep-alive")
   .header("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197")
   .field("file", new File("E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav"))
   .field("temperature", "0.2")
   .field("response-format", "json")
   .field("audio_format", "wav")
   .asString();

```
## OkHttp
```
OkHttpClient client = new OkHttpClient().newBuilder()
   .build();
MediaType mediaType = MediaType.parse("multipart/form-data; boundary=--------------------------671827497522367123871197");
RequestBody body = new MultipartBody.Builder().setType(MultipartBody.FORM)
   .addFormDataPart("file","E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav",
      RequestBody.create(MediaType.parse("application/octet-stream"),
      new File("E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav")))
   .addFormDataPart("temperature","0.2")
   .addFormDataPart("response-format","json")
   .addFormDataPart("audio_format","wav")
   .build();
Request request = new Request.Builder()
   .url("http://192.168.3.9:8080/inference")
   .method("POST", body)
   .addHeader("User-Agent", "Apifox/1.0.0 (https://apifox.com)")
   .addHeader("Accept", "*/*")
   .addHeader("Host", "192.168.3.9:8080")
   .addHeader("Connection", "keep-alive")
   .addHeader("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197")
   .build();
Response response = client.newCall(request).execute();
```

# swift
## URLSession
```
import Foundation
#if canImport(FoundationNetworking)
import FoundationNetworking
#endif

var semaphore = DispatchSemaphore (value: 0)

let parameters = [
   [
      "key": "file",
      "src": "E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav",
      "type": "file"
   ],
   [
      "key": "temperature",
      "value": "0.2",
      "type": "text"
   ],
   [
      "key": "response-format",
      "value": "json",
      "type": "text"
   ],
   [
      "key": "audio_format",
      "value": "wav",
      "type": "text"
   ]] as [[String : Any]]

let boundary = "Boundary-\(UUID().uuidString)"
var body = ""
var error: Error? = nil
for param in parameters {
   if param["disabled"] == nil {
      let paramName = param["key"]!
      body += "--\(boundary)\r\n"
      body += "Content-Disposition:form-data; name=\"\(paramName)\""
      if param["contentType"] != nil {
         body += "\r\nContent-Type: \(param["contentType"] as! String)"
      }
      let paramType = param["type"] as! String
      if paramType == "text" {
         let paramValue = param["value"] as! String
         body += "\r\n\r\n\(paramValue)\r\n"
      } else {
         let paramSrc = param["src"] as! String
         let fileData = try NSData(contentsOfFile:paramSrc, options:[]) as Data
         let fileContent = String(data: fileData, encoding: .utf8)!
         body += "; filename=\"\(paramSrc)\"\r\n"
           + "Content-Type: \"content-type header\"\r\n\r\n\(fileContent)\r\n"
      }
   }
}
body += "--\(boundary)--\r\n";
let postData = body.data(using: .utf8)

var request = URLRequest(url: URL(string: "http://192.168.3.9:8080/inference")!,timeoutInterval: Double.infinity)
request.addValue("Apifox/1.0.0 (https://apifox.com)", forHTTPHeaderField: "User-Agent")
request.addValue("*/*", forHTTPHeaderField: "Accept")
request.addValue("192.168.3.9:8080", forHTTPHeaderField: "Host")
request.addValue("keep-alive", forHTTPHeaderField: "Connection")
request.addValue("multipart/form-data; boundary=--------------------------671827497522367123871197", forHTTPHeaderField: "Content-Type")
request.addValue("multipart/form-data; boundary=\(boundary)", forHTTPHeaderField: "Content-Type")

request.httpMethod = "POST"
request.httpBody = postData

let task = URLSession.shared.dataTask(with: request) { data, response, error in 
   guard let data = data else {
      print(String(describing: error))
      semaphore.signal()
      return
   }
   print(String(data: data, encoding: .utf8)!)
   semaphore.signal()
}

task.resume()
semaphore.wait()
```

# Go
## native
```
package main

import (
   "fmt"
   "bytes"
   "mime/multipart"
   "os"
   "path/filepath"
   "io"
   "net/http"
   "io/ioutil"
)

func main() {

   url := "http://192.168.3.9:8080/inference"
   method := "POST"

   payload := &bytes.Buffer{}
   writer := multipart.NewWriter(payload)
   file, errFile1 := os.Open("E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav")
   defer file.Close()
   part1,
         errFile1 := writer.CreateFormFile("file",filepath.Base("E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav"))
   _, errFile1 = io.Copy(part1, file)
   if errFile1 != nil {
      fmt.Println(errFile1)
      return
   }
   _ = writer.WriteField("temperature", "0.2")
   _ = writer.WriteField("response-format", "json")
   _ = writer.WriteField("audio_format", "wav")
   err := writer.Close()
   if err != nil {
      fmt.Println(err)
      return
   }


   client := &http.Client {
   }
   req, err := http.NewRequest(method, url, payload)

   if err != nil {
      fmt.Println(err)
      return
   }
   req.Header.Add("User-Agent", "Apifox/1.0.0 (https://apifox.com)")
   req.Header.Add("Accept", "*/*")
   req.Header.Add("Host", "192.168.3.9:8080")
   req.Header.Add("Connection", "keep-alive")
   req.Header.Add("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197")

   req.Header.Set("Content-Type", writer.FormDataContentType())
   res, err := client.Do(req)
   if err != nil {
      fmt.Println(err)
      return
   }
   defer res.Body.Close()

   body, err := ioutil.ReadAll(res.Body)
   if err != nil {
      fmt.Println(err)
      return
   }
   fmt.Println(string(body))
}
```

# PHP

## cUrl
```
<?php

$curl = curl_init();

curl_setopt_array($curl, array(
   CURLOPT_URL => 'http://192.168.3.9:8080/inference',
   CURLOPT_RETURNTRANSFER => true,
   CURLOPT_ENCODING => '',
   CURLOPT_MAXREDIRS => 10,
   CURLOPT_TIMEOUT => 0,
   CURLOPT_FOLLOWLOCATION => true,
   CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
   CURLOPT_CUSTOMREQUEST => 'POST',
   CURLOPT_POSTFIELDS => array('file'=> new CURLFILE('E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav'),,,),
   CURLOPT_HTTPHEADER => array(
      'User-Agent: Apifox/1.0.0 (https://apifox.com)',
      'Accept: */*',
      'Host: 192.168.3.9:8080',
      'Connection: keep-alive',
      'Content-Type: multipart/form-data; boundary=--------------------------671827497522367123871197'
   ),
));

$response = curl_exec($curl);

curl_close($curl);
echo $response;

```

## Http_Request2
```
<?php
require_once 'HTTP/Request2.php';
$request = new HTTP_Request2();
$request->setUrl('http://192.168.3.9:8080/inference');
$request->setMethod(HTTP_Request2::METHOD_POST);
$request->setConfig(array(
   'follow_redirects' => TRUE
));
$request->setHeader(array(
   'User-Agent' => 'Apifox/1.0.0 (https://apifox.com)',
   'Accept' => '*/*',
   'Host' => '192.168.3.9:8080',
   'Connection' => 'keep-alive',
   'Content-Type' => 'multipart/form-data; boundary=--------------------------671827497522367123871197'
));
$request->addUpload('file', 'E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav', 'E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav', '<Content-Type Header>');
try {
   $response = $request->send();
   if ($response->getStatus() == 200) {
      echo $response->getBody();
   }
   else {
      echo 'Unexpected HTTP status: ' . $response->getStatus() . ' ' .
      $response->getReasonPhrase();
   }
}
catch(HTTP_Request2_Exception $e) {
   echo 'Error: ' . $e->getMessage();
}
```

## pecl_http
```
<?php
$client = new http\Client;
$request = new http\Client\Request;
$request->setRequestUrl('http://192.168.3.9:8080/inference');
$request->setRequestMethod('POST');
$body = new http\Message\Body;
$body->addForm(array(

), array(
      array('name' => 'file', 'type' => '<Content-type header>', 'file' => 'E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav', 'data' => null)
));
$request->setBody($body);
$request->setOptions(array());
$request->setHeaders(array(
   'User-Agent' => 'Apifox/1.0.0 (https://apifox.com)',
   'Accept' => '*/*',
   'Host' => '192.168.3.9:8080',
   'Connection' => 'keep-alive',
   'Content-Type' => 'multipart/form-data; boundary=--------------------------671827497522367123871197'
));
$client->enqueue($request)->send();
$response = $client->getResponse();
echo $response->getBody();
```
## Guzzle
```
<?php
$client = new Client();
$headers = [
   'User-Agent' => 'Apifox/1.0.0 (https://apifox.com)',
   'Accept' => '*/*',
   'Host' => '192.168.3.9:8080',
   'Connection' => 'keep-alive',
   'Content-Type' => 'multipart/form-data; boundary=--------------------------671827497522367123871197'
];
$options = [
   'multipart' => [
      [
         'name' => 'file',
         'contents' => Utils::tryFopen('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav', 'r'),
         'filename' => 'E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav',
         'headers'  => [
            'Content-Type' => '<Content-type header>'
         ]
      ],
      [
         'name' => 'temperature',
         'contents' => '0.2'
      ],
      [
         'name' => 'response-format',
         'contents' => 'json'
      ],
      [
         'name' => 'audio_format',
         'contents' => 'wav'
      ]
]];
$request = new Request('POST', 'http://192.168.3.9:8080/inference', $headers);
$res = $client->sendAsync($request, $options)->wait();
echo $res->getBody();

```
# Python
## Http.client
```
import http.client
import mimetypes
from codecs import encode

conn = http.client.HTTPSConnection("192.168.3.9", 8080)
dataList = []
boundary = 'wL36Yn8afVp8Ag7AmP8qZ0SA4n1v9T'
dataList.append(encode('--' + boundary))
dataList.append(encode('Content-Disposition: form-data; name=file; filename={0}'.format('E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav')))

fileType = mimetypes.guess_type('E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav')[0] or 'application/octet-stream'
dataList.append(encode('Content-Type: {}'.format(fileType)))
dataList.append(encode(''))

with open('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav', 'rb') as f:
   dataList.append(f.read())
dataList.append(encode('--' + boundary))
dataList.append(encode('Content-Disposition: form-data; name=temperature;'))

dataList.append(encode('Content-Type: {}'.format('text/plain')))
dataList.append(encode(''))

dataList.append(encode("0.2"))
dataList.append(encode('--' + boundary))
dataList.append(encode('Content-Disposition: form-data; name=response-format;'))

dataList.append(encode('Content-Type: {}'.format('text/plain')))
dataList.append(encode(''))

dataList.append(encode("json"))
dataList.append(encode('--' + boundary))
dataList.append(encode('Content-Disposition: form-data; name=audio_format;'))

dataList.append(encode('Content-Type: {}'.format('text/plain')))
dataList.append(encode(''))

dataList.append(encode("wav"))
dataList.append(encode('--'+boundary+'--'))
dataList.append(encode(''))
body = b'\r\n'.join(dataList)
payload = body
headers = {
   'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
   'Accept': '*/*',
   'Host': '192.168.3.9:8080',
   'Connection': 'keep-alive',
   'Content-Type': 'multipart/form-data; boundary=--------------------------671827497522367123871197',
   'Content-type': 'multipart/form-data; boundary={}'.format(boundary)
}
conn.request("POST", "/inference", payload, headers)
res = conn.getresponse()
data = res.read()
print(data.decode("utf-8"))
```
## Requests
```
import requests

url = "http://192.168.3.9:8080/inference"

payload={}
files=[
   ('file',('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav',open('E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav','rb'),'audio/wav'))
]
headers = {
   'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
   'Accept': '*/*',
   'Host': '192.168.3.9:8080',
   'Connection': 'keep-alive',
   'Content-Type': 'multipart/form-data; boundary=--------------------------671827497522367123871197'
}

response = requests.request("POST", url, headers=headers, data=payload, files=files)

print(response.text)
```

# HTTP
## Http
```
POST /inference HTTP/1.1
Host: 192.168.3.9:8080
User-Agent: Apifox/1.0.0 (https://apifox.com)
Accept: */*
Host: 192.168.3.9:8080
Connection: keep-alive
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW

----WebKitFormBoundary7MA4YWxkTrZu0gW
Content-Disposition: form-data; name="file"; filename="E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav"
Content-Type: audio/wav

(data)
----WebKitFormBoundary7MA4YWxkTrZu0gW
----WebKitFormBoundary7MA4YWxkTrZu0gW
----WebKitFormBoundary7MA4YWxkTrZu0gW
----WebKitFormBoundary7MA4YWxkTrZu0gW
```
# C
## libcurl
```
CURL *curl;
CURLcode res;
curl = curl_easy_init();
if(curl) {
   curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
   curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.3.9:8080/inference");
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
   struct curl_slist *headers = NULL;
   headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
   headers = curl_slist_append(headers, "Accept: */*");
   headers = curl_slist_append(headers, "Host: 192.168.3.9:8080");
   headers = curl_slist_append(headers, "Connection: keep-alive");
   headers = curl_slist_append(headers, "Content-Type: multipart/form-data; boundary=--------------------------671827497522367123871197");
   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
   curl_mime *mime;
   curl_mimepart *part;
   mime = curl_mime_init(curl);
   part = curl_mime_addpart(mime);
   curl_mime_name(part, "file");
   curl_mime_filedata(part, "E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav");
   part = curl_mime_addpart(mime);
   curl_mime_name(part, "temperature");
   curl_mime_data(part, "0.2", CURL_ZERO_TERMINATED);
   part = curl_mime_addpart(mime);
   curl_mime_name(part, "response-format");
   curl_mime_data(part, "json", CURL_ZERO_TERMINATED);
   part = curl_mime_addpart(mime);
   curl_mime_name(part, "audio_format");
   curl_mime_data(part, "wav", CURL_ZERO_TERMINATED);
   curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
   res = curl_easy_perform(curl);
   curl_mime_free(mime);
}
curl_easy_cleanup(curl);

```
# C#
## RestSharp
```
var client = new RestClient("http://192.168.3.9:8080/inference");
client.Timeout = -1;
var request = new RestRequest(Method.POST);
client.UserAgent = "Apifox/1.0.0 (https://apifox.com)";
request.AddHeader("Accept", "*/*");
request.AddHeader("Host", "192.168.3.9:8080");
request.AddHeader("Connection", "keep-alive");
request.AddHeader("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197");
request.AddFile("file", "E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav");
request.AddParameter("temperature", "0.2");
request.AddParameter("response-format", "json");
request.AddParameter("audio_format", "wav");
IRestResponse response = client.Execute(request);
Console.WriteLine(response.Content);
```

# Objective-C
## NSURLsession
```
var client = new RestClient("http://192.168.3.9:8080/inference");
client.Timeout = -1;
var request = new RestRequest(Method.POST);
client.UserAgent = "Apifox/1.0.0 (https://apifox.com)";
request.AddHeader("Accept", "*/*");
request.AddHeader("Host", "192.168.3.9:8080");
request.AddHeader("Connection", "keep-alive");
request.AddHeader("Content-Type", "multipart/form-data; boundary=--------------------------671827497522367123871197");
request.AddFile("file", "E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav");
request.AddParameter("temperature", "0.2");
request.AddParameter("response-format", "json");
request.AddParameter("audio_format", "wav");
IRestResponse response = client.Execute(request);
Console.WriteLine(response.Content);
```
# Ruby
## Net::HTTP
```
require "uri"
require "net/http"

url = URI("http://192.168.3.9:8080/inference")

http = Net::HTTP.new(url.host, url.port);
request = Net::HTTP::Post.new(url)
request["User-Agent"] = "Apifox/1.0.0 (https://apifox.com)"
request["Accept"] = "*/*"
request["Host"] = "192.168.3.9:8080"
request["Connection"] = "keep-alive"
request["Content-Type"] = "multipart/form-data; boundary=--------------------------671827497522367123871197"
form_data = [['file', File.open('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav')],['temperature', File.open('undefined')],['response-format', File.open('undefined')],['audio_format', File.open('undefined')]]
request.set_form form_data, 'multipart/form-data'
response = http.request(request)
puts response.read_body

```
# OCML
## CoHttp
```
open Lwt
open Cohttp
open Cohttp_lwt_unix

let parameters = [|
   [| ("name", "file"); ("fileName", "E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav") |];
   [| ("name", "temperature"); ("value", "0.2") |];
   [| ("name", "response-format"); ("value", "json") |];
   [| ("name", "audio_format"); ("value", "wav") |]
|];;
let boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";;
let postData = ref "";;

for x = 0 to Array.length parameters - 1 do
   let (_, paramName) = parameters.(x).(0) in
   let (paramType, _) = parameters.(x).(1) in
   let accum = "--" ^ boundary ^ "\r\n" ^ "Content-Disposition: form-data; name=\"" ^ paramName ^ "\"" in
   if paramType = "value" then (
      let (_, paramValue) = parameters.(x).(1) in
      postData := if Array.length parameters.(x) == 3 then (
         let (_, contentType) = parameters.(x).(2) in
         !postData ^ accum ^ "\r\n" ^ "Content-Type: " ^ contentType ^ "\r\n\r\n" ^ paramValue ^ "\r\n"
      ) else (
         !postData ^ accum ^ "\r\n\r\n" ^ paramValue ^ "\r\n"
      );
   )
   else if paramType = "fileName" then (
      let (_, filepath) = parameters.(x).(1) in
      postData := !postData ^ accum ^ "; filename=\""^ filepath ^"\"\r\n";
      let ch = open_in filepath in
         let fileContent = really_input_string ch (in_channel_length ch) in
         close_in ch;
      postData := !postData ^ "Content-Type: {content-type header}\r\n\r\n"^ fileContent ^"\r\n";
   )
done;;
postData := !postData ^ "--" ^ boundary ^ "--"

let reqBody = 
   let uri = Uri.of_string "http://192.168.3.9:8080/inference" in
   let headers = Header.init ()
      |> fun h -> Header.add h "User-Agent" "Apifox/1.0.0 (https://apifox.com)"
      |> fun h -> Header.add h "Accept" "*/*"
      |> fun h -> Header.add h "Host" "192.168.3.9:8080"
      |> fun h -> Header.add h "Connection" "keep-alive"
      |> fun h -> Header.add h "Content-Type" "multipart/form-data; boundary=--------------------------671827497522367123871197"
      |> fun h -> Header.add h "content-type" "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW"
   in
   let body = Cohttp_lwt.Body.of_string !postData in

   Client.call ~headers ~body `POST uri >>= fun (_resp, body) ->
   body |> Cohttp_lwt.Body.to_string >|= fun body -> body

let () =
   let respBody = Lwt_main.run reqBody in
   print_endline (respBody)
```
# Dart
## http
```
open Lwt
open Cohttp
open Cohttp_lwt_unix

let parameters = [|
   [| ("name", "file"); ("fileName", "E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav") |];
   [| ("name", "temperature"); ("value", "0.2") |];
   [| ("name", "response-format"); ("value", "json") |];
   [| ("name", "audio_format"); ("value", "wav") |]
|];;
let boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";;
let postData = ref "";;

for x = 0 to Array.length parameters - 1 do
   let (_, paramName) = parameters.(x).(0) in
   let (paramType, _) = parameters.(x).(1) in
   let accum = "--" ^ boundary ^ "\r\n" ^ "Content-Disposition: form-data; name=\"" ^ paramName ^ "\"" in
   if paramType = "value" then (
      let (_, paramValue) = parameters.(x).(1) in
      postData := if Array.length parameters.(x) == 3 then (
         let (_, contentType) = parameters.(x).(2) in
         !postData ^ accum ^ "\r\n" ^ "Content-Type: " ^ contentType ^ "\r\n\r\n" ^ paramValue ^ "\r\n"
      ) else (
         !postData ^ accum ^ "\r\n\r\n" ^ paramValue ^ "\r\n"
      );
   )
   else if paramType = "fileName" then (
      let (_, filepath) = parameters.(x).(1) in
      postData := !postData ^ accum ^ "; filename=\""^ filepath ^"\"\r\n";
      let ch = open_in filepath in
         let fileContent = really_input_string ch (in_channel_length ch) in
         close_in ch;
      postData := !postData ^ "Content-Type: {content-type header}\r\n\r\n"^ fileContent ^"\r\n";
   )
done;;
postData := !postData ^ "--" ^ boundary ^ "--"

let reqBody = 
   let uri = Uri.of_string "http://192.168.3.9:8080/inference" in
   let headers = Header.init ()
      |> fun h -> Header.add h "User-Agent" "Apifox/1.0.0 (https://apifox.com)"
      |> fun h -> Header.add h "Accept" "*/*"
      |> fun h -> Header.add h "Host" "192.168.3.9:8080"
      |> fun h -> Header.add h "Connection" "keep-alive"
      |> fun h -> Header.add h "Content-Type" "multipart/form-data; boundary=--------------------------671827497522367123871197"
      |> fun h -> Header.add h "content-type" "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW"
   in
   let body = Cohttp_lwt.Body.of_string !postData in

   Client.call ~headers ~body `POST uri >>= fun (_resp, body) ->
   body |> Cohttp_lwt.Body.to_string >|= fun body -> body

let () =
   let respBody = Lwt_main.run reqBody in
   print_endline (respBody)
```

# R
## httr
```
library(httr)

headers = c(
   'User-Agent' = 'Apifox/1.0.0 (https://apifox.com)',
   'Accept' = '*/*',
   'Host' = '192.168.3.9:8080',
   'Connection' = 'keep-alive',
   'Content-Type' = 'multipart/form-data; boundary=--------------------------671827497522367123871197'
)

body = list(
   'file' = upload_file('E:\code\cpp\cpp-study\cpp-study-clion\audio\jfk.wav'),
   'temperature' = '0.2',
   'response-format' = 'json',
   'audio_format' = 'wav'
)

res <- VERB("POST", url = "http://192.168.3.9:8080/inference", body = body, add_headers(headers), encode = 'multipart')

cat(content(res, 'text'))
```

## Rcurl
```
library(RCurl)
headers = c(
   "User-Agent" = "Apifox/1.0.0 (https://apifox.com)",
   "Accept" = "*/*",
   "Host" = "192.168.3.9:8080",
   "Connection" = "keep-alive",
   "Content-Type" = "multipart/form-data; boundary=--------------------------671827497522367123871197"
)
params = c(
   "temperature" = "0.2",
   "response-format" = "json",
   "audio_format" = "wav"
)
file0 = fileUpload(
   filename = path.expand('E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav'))
res <- postForm("http://192.168.3.9:8080/inference", file = file0, .params = params, .opts=list(httpheader = headers, followlocation = TRUE), style = "httppost")
cat(res)
```
