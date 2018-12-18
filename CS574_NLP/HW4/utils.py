# -*- coding: utf-8 -*-
from polyglot.text import Text
import re

blob = '20세기 폭스 필름 코퍼레이션 (영어: 20th Century Fox Film Corporation)은 미국의 6대 메이저 영화사 중의 하나이다. 이 영화사는 줄여서 20세기 폭스라고 불리며 오스트레일리아 출신의 루퍼트 머독의 21세기 폭스의 폭스 엔터테인먼트 그룹에 속하는 회사이다. 20세기 폭스는 현재 북미 점유율 3위를 기록하고 있다. 20세기 폭스는 수많은 흥행작들을 내놓았는데 대표적인 작품으로는 사운드 오브 뮤직, 스타워즈, 엑스맨 등이 있다. 또한 1950년대 최고의 배우였던 마릴린 먼로의 전속 회사로도 유명하다. 20세기 폭스는 최근에 아바타 배급을 맡은 것으로도 알려져 있다.'
text = Text(blob, hint_language_code='ko')
print(text.entities)
for ent in text.entities:
    for e in ent:
        print(e.encode('utf-8'))




