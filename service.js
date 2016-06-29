// Adapted from from https://nodejs.org/en/docs/guides/anatomy-of-an-http-transaction/

const http = require('http');

var server = http.createServer((request, response) => {
  // TODO: Improve error handling
  request.on('error', console.error);
  response.on('error', console.error);
  response.writeHead(200, {'Content-Type': request.headers['Content-Type']});
  request.on('data', response.write.bind(response));
  request.on('end', response.end.bind(response));
}).listen(8080);
