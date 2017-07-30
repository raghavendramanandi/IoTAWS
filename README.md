# IoTAWS

- ESP8266 (NodeMCU)
- Arduino IDE
- LDR
- AWS Lambda
- DynamoDB


Public API: To update the light status

https://k2l9nx7gw8.execute-api.us-west-1.amazonaws.com/prod/UpdateStatus?team=p4r&lightStatus=off&deviceId=1

Integration Request: Response Body Mapping

{
    "deviceId": "$input.parms('deviceId')",
    "team": "$input.parms('team')",
    "lightStatus": "$input.parms('lightStatus')"
}

Respurces details:
- Lambda : N. California
- API Gateway : N. California
- EC2 : Ohio
- DynamoBD : N. California
