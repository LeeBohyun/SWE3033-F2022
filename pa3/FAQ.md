# FAQ for Project 3

Q1. benchmark_directFillRand에 제공된 코드가 랜덤한 (Key, Value) 쌍을 만들어 Insert 하는데요.
(1) 과제에서 요구하는 내용은 해당 함수에서 num_ 만큼 direct execution을 반복적으로 수행하는 게 맞나요?
```
네 맞습니다. 
```

맞다면, (2) Key가 여러 번 중복되는 경우가 있는데 이때 어떻게 수행해야 하는건가요?
```
key 중복되는 경우는 신경쓰지말고 구현하시면 됩니다.
```

Q2. Primary key 가 중복된다면 skip 하나요?
그렇다면 실제로 수행된 횟수를 계산하여 실제 througput을 계산해서 마지막에 출력하면 되나요?
```
key 중복은 신경안쓰시고 하면 됩니다. skip안해도 됩니다. 
throughput 계산도 해당 경우는 고려안하셔도 괜찮습니다.
```

Q3. Primary key 가 중복된다면 해당 key에 대한 value를 갱신하기 위해 UPDATE 수행해야 하나요? 
이 경우는 랜덤 값의 Key가 얼마나 중복되는지에 따라 UPDATE 수행 횟수가 매번 달라지기 때문에 공정한 벤치마크 성능 판단이 어려울 것 같아, 올바른 접근은 아닌 것 같다고 생각합니다.
```
네 맞습니다.
이를 해결하기 위해서는 데이터베이스 테이블 구조를 변경해야합니다. 
변경해서 적용하는 것도 contribution이 될 수 있습니다. 한번 시도해보세요 :)
본 프로젝트의 목표는 SQLite API를 활용한 구현이 목적이 있어서 현재 데이터베이스 파일을 이용해서 구현해도 무방합니다.
```

Q4. 함수 내에서 1번만 SQL INSERT를 수행하는건가요?
```
한번만 수행하는 것이 아니라, 매개변수로 주어진 `num_` 만큼 수행해야 합니다.
```


Q5. 최종 제출물(압축파일)에는 homework.cc 파일, command 결과 스크린샷, 프레젠테이션 영상 3개만 들어있으면 되는게 맞을까요? 따로 report 작성은 하지 않아도 되는게 맞을까요?
```
네 report작성은 발표 프레젠테이션 영상으로 대체됩니다.
```

Q6. PPT에 포함될 내용 중 Motivation의 경우, own benchmark function에 대한 motivation인가요 혹은 제시된 3개의 함수까지 포함한 전체적인 motivation인가요?
```
Motivation에 대한 범위는 "SQLite Benchmark에 대한 motivation" 을 자유롭게 작성/발표해주시면 됩니다.
```

Q6. PPT에 포함될 내용 중 Motivation의 경우, own benchmark function에 대한 motivation인가요 혹은 제시된 3개의 함수까지 포함한 전체적인 motivation인가요?
```
네 맞습니다. 구체적으로, 기본 homework.cc 에 제공된 함수 + 본인이 새롭게 제한하는 함수에 대한 설명을 해주시면 됩니다.
```

Q7. PPT에 포함될 내용 중 Design of sqlite3Bench 의 경우, README.md의 3. About SQLiteBench 를 풀어서 설명하는 느낌이 맞을까요?
```
발표영상 길이에는 따로 제한이 없습니다. (10~15분 정도면 충분할 것 같습니다. 수업때 따로 재공지드리겠습니다.)
추가적으로, 발표 형식, 분량 제한은 전혀 없습니다. 
```
