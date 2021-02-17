


const express = require('express')
const app = express()


var a1 = [];
var AWS = require("aws-sdk");
let awsConfig = {
    "region": "ap-northeast-2",
    "endpoint": "http://dynamodb.ap-northeast-2.amazonaws.com",
    "accessKeyId": "AKIA272ZVUOEA5MKP3SK", "secretAccessKey": "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"
};

var temp = '';

AWS.config.update(awsConfig);

var params = {
    TableName: "trash",
};

app.get('/',function(req,res) {
let docClient = new AWS.DynamoDB.DocumentClient();
docClient.scan(params, function (err, data) {
        if (err) {
            console.log("users::fetchOneByKey::error - " + JSON.stringify(err, null, 2));
        }
        else {
            //console.log(data['Items']);
            //console.log(data['Items']);
            //console.log(data['Items']['state']);
        }
}).promise()
.then(function(data) {
    console.log(data['Items']);
    res.json(data['Items'])
})
})

app.listen(3000)