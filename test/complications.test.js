var c=require('../src/pkjs/complications.js'),a=require('assert');
var d=c.toDisplay;
a.deepStrictEqual(d({numericValue:{value:50,min:0,max:200,unit:'x'}}),{percent:25,label:'50x'});
a.deepStrictEqual(d({numericValue:{value:84,unit:'%',min:0,max:100},shortText:{text:'84%'}}),{percent:84,label:'84%'});
a.deepStrictEqual(d({numericValue:{value:70,unit:'%'}}),{percent:70,label:'70%'});
a.deepStrictEqual(d({numericValue:{value:21.46,unit:'C'}}),{percent:0,label:'21.5C'});
a.deepStrictEqual(d({numericValue:{value:500,min:0,max:100}}).percent,100);
a.deepStrictEqual(d({boolean:{value:true},numericValue:{value:10,min:0,max:20}}).percent,50);
a.deepStrictEqual(d({boolean:{value:true}}),{percent:100,label:'On'});
a.deepStrictEqual(d({boolean:{value:false}}),{percent:0,label:'Off'});
a.deepStrictEqual(d({longText:{text:'hi'}}),c.UNSUPPORTED);
a.deepStrictEqual(d(undefined),c.UNSUPPORTED);
console.log('ok')
