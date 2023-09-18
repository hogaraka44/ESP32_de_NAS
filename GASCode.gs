function doGet() {
  return HtmlService.createHtmlOutput(read_text());
}

function doPost(nondef)
{
  const DOC_URL = 'https://docs.google.com/document/d/<id>/edit'; 
  const doc = DocumentApp.openByUrl(DOC_URL);
  //ドキュメントの内容を取得する
  const body = doc.getBody();
  //ドキュメントにテキストを書き込む
  body.clear() // 全消去
  
  var e = JSON.stringify(nondef);//string型　//str <- e
  var obj2 = JSON.parse(e);//obj型に直す //obj<-e
  
  console.log("obj2.postData.contentsの型は"+typeof obj2.postData.contents);
  console.log(obj2.postData.contents);

  body.appendParagraph(obj2.postData.contents);
}

//ドキュメントから文字を得る関数
function read_text()
{
  const DOC_URL = 'https://docs.google.com/document/d/<id>/edit'; 
  const doc = DocumentApp.openByUrl(DOC_URL);
  
  console.log(doc.getBody().getText());

  return doc.getBody().getText();
}
