from pipe_reader_writer import PipeReaderWriter
from paths import driver_path, browser_path, profile_path

from html.parser import HTMLParser
from datetime import datetime

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service

import sys
import time
import random

def debug_print(s):
    time_str = datetime.now().strftime("%H:%M:%S")
    print(f'[{time_str}]  {s}', file=sys.stderr)

def sanitize(name: str) -> str:
    return (''.join(filter(lambda x: x.isalpha() or x.isdigit() or x == ',' or x == ' ', name.lower()))).strip()

def sanitize_all(names: set[str]) -> set[str]:
    return set(map(sanitize, names))

class BGAParser(HTMLParser):
    def reset_state(self):
        self.object_names = {}
        self.name_objects = {}
        self.object_finders = {}
        self.card_types = {}
        self.can_have_built_wonders = False
        self.start_parse()

    def start_parse(self):
        self.stack = []
        self.obj_stack = []
        self.last_object = None
        self.pyramid_poses_cards = set()
        self.guild_poses = set()
        self.built_cards = set()
        self.discarded_cards = set()
        self.game_tokens = set()
        self.box_tokens = set()
        self.built_tokens = set()
        self.revealed_wonders = set()
        self.selected_wonders = set()
        self.built_wonders = set()

    def handle_card(self):
        if 'data-building-id' not in self.stack[-1]:
            return

        container = self.stack[-3]
        building_id = self.stack[-1]['data-building-id']
        self.last_object = f'card_{building_id}'

        if self.stack[-1]['id'] != '':
            self.object_finders[self.last_object] = (By.ID, self.stack[-1]['id'])
        else:
            self.object_finders[self.last_object] = (By.XPATH, f'//*[@id="" and @data-building-id="{building_id}"]')

        if 'data-building-type' in self.stack[-1]:
            card_type = sanitize(self.stack[-1]['data-building-type'])
            if card_type == 'purple':
                card_type = 'guild'
            self.card_types[self.last_object] = card_type

        if 'building building_small' not in self.stack[-1]['class']:
            self.built_cards.add(self.last_object)
        elif container['id'] == 'draftpool_container':
            pos = int(self.stack[-1]['data-location'])
            type = self.stack[-1]['data-building-type']
            if type == 'Purple' or 'background-position: -100% -700%' in self.stack[-1]['style']:
                self.guild_poses.add(pos)
            if type != '':
                self.pyramid_poses_cards.add((pos, self.last_object))
        elif container['id'] == 'discarded_cards_container':
            self.discarded_cards.add(self.last_object)

    def handle_token(self):
        container = self.stack[-3]
        token_id = self.stack[-1]['data-progress-token-id']
        self.last_object = f'token_{token_id}'
        self.object_finders[self.last_object] = (By.ID, self.stack[-1]['id'])

        if 'id' not in container:
            self.built_tokens.add(self.last_object)
        elif container['id'] == 'board_progress_tokens':
            self.game_tokens.add(self.last_object)
        elif container['id'] == 'progress_token_from_box_container':
            self.box_tokens.add(self.last_object)

    def handle_wonder(self):
        container = self.stack[-4]

        if 'id' not in container:
            return

        built = self.stack[-1]['data-constructed']
        wonder_id = self.stack[-1]['data-wonder-id']
        self.last_object = f'wonder_{wonder_id}'
        self.object_finders[self.last_object] = (By.ID, self.stack[-1]['id'])

        if container['id'] == 'wonder_selection_container':
            self.revealed_wonders.add(self.last_object)
        elif 'player_wonders' in container['id']:
            if built == '0':
                self.selected_wonders.add(self.last_object)
            else:
                self.built_wonders.add(self.last_object)

    def handle_starttag(self, tag, attrs):
        if tag != 'div':
            return

        attrs = dict(attrs)
        self.stack.append(attrs)

        self.last_object = None
        self.obj_stack.append(None)

        if 'class' not in attrs:
            return

        if 'building building_small' in attrs['class'] or 'building building_header_small' in attrs['class']:
            self.handle_card()

        if 'progress_token progress_token_small' in attrs['class']:
            self.handle_token()

        if 'wonder wonder_small' in attrs['class']:
            self.handle_wonder()

        self.obj_stack[-1] = self.last_object

    def handle_endtag(self, tag):
        if tag != 'div':
            return

        self.stack.pop()
        self.obj_stack.pop()

    def handle_data(self, data):
        if data is None:
            return

        data = sanitize(data)

        if data == '':
            return

        if len(self.obj_stack) > 0 and self.obj_stack[-1] is not None:
            self.object_names[self.obj_stack[-1]] = data.strip()
            self.last_object = None

    def map_object_names(self, obj_set):
        res = set()
        for obj in obj_set:
            name = self.object_names.get(obj)
            if name is None:
                debug_print(f'No name known for {obj}')
                continue
            self.name_objects[name] = obj
            res.add(name)
        return res

    def map_pos_object_names(self, obj_set):
        res = set()
        for pos, obj in obj_set:
            name = self.object_names.get(obj)
            if name is None:
                debug_print(f'No name known for {obj}')
                continue
            self.name_objects[name] = obj
            res.add((pos, name))
        return res

    def get_state(self):
        if len(self.selected_wonders) > 0 and len(self.built_wonders) > 0:
            self.can_have_built_wonders = True

        if not self.can_have_built_wonders:
            self.selected_wonders.update(self.built_wonders)
            self.built_wonders = set()

        return {
            'pyramid_poses_cards' : self.map_pos_object_names(self.pyramid_poses_cards),
            'guild_poses' : self.guild_poses,
            'built_cards' : self.map_object_names(self.built_cards),
            'discarded_cards' : self.map_object_names(self.discarded_cards),
            'game_tokens' : self.map_object_names(self.game_tokens),
            'box_tokens' : self.map_object_names(self.box_tokens),
            'built_tokens' : self.map_object_names(self.built_tokens),
            'revealed_wonders' : self.map_object_names(self.revealed_wonders),
            'selected_wonders' : self.map_object_names(self.selected_wonders),
            'built_wonders' : self.map_object_names(self.built_wonders)
        }

class BGAGame:

    NONE = -100

    PLAYER_ME = 'Player 1'
    PLAYER_OTHER = 'Player 2'

    OTHER_PLAYER = {
        'Player 1' : 'Player 2',
        'Player 2' : 'Player 1'
    }

    ALL_TOKENS = {
        'Agriculture',
        'Architecture',
        'Economy',
        'Law',
        'Masonry',
        'Mathematics',
        'Philosophy',
        'Strategy',
        'Theology',
        'Urbanism'
    }

    PYRAMID_SIZE = 20

    STATE_CHANGED = 1
    STATE_UNCHANGED = 0
    STATE_INVALID = -1

    MAX_INVALID_CNT = 30

    def __init__(self, pipe : PipeReaderWriter):
        self.pipe = pipe

        options = Options()
        options.binary_location = browser_path
        options.add_argument(f'--profile {profile_path}')
        self.driver = webdriver.Firefox(service=Service(driver_path), options=options)

        self.parser = BGAParser()

        self.reset_state()

    def find_title_text(self) -> str:
        title = self.driver.find_element(By.ID, 'pagemaintitletext')
        title_text = sanitize(title.text)
        return title_text

    def parse_curr_player(self, title_text):
        if 'must' not in title_text:
            return None
        elif 'you' in title_text:
            return BGAGame.PLAYER_ME
        else:
            return BGAGame.PLAYER_OTHER

    def open_bga_page(self) -> None:
        self.driver.get("https://boardgamearena.com/lobby")

    def select_by_finder(self, finder) -> None:
        time.sleep(0.1)
        try:
            self.driver.find_element(By.ID, 'pagemaintitletext').click()
        except Exception:
            pass
        debug_print(f'Trying to select: {finder}')
        element = self.driver.find_element(*finder)
        debug_print(f'Got element: {finder}')
        if random.random() < 0.5:
            self.driver.execute_script("if (window.getComputedStyle(arguments[0]).display !== 'none') { arguments[0].click(); }", element)
        else:
            element.click()
        debug_print(f'Done trying to click: {finder}')

    def select_by_id(self, id: str) -> None:
        self.select_by_finder((By.ID, id))

    def select_by_name(self, name: str) -> None:
        self.select_by_finder(self.parser.object_finders[self.parser.name_objects[name]])

    def create_game_btn(self) -> None:
        self.select_by_id('joingame_create_1266')

    def open_table_btn(self) -> None:
        self.select_by_id('open_table_now')

    def start_game_btn(self) -> None:
        self.select_by_id('startgame')

    def accept_game_btn(self) -> None:
        self.select_by_id('ags_start_game_accept')

    def build_card(self, name: str) -> None:
        self.select_by_name(name)
        self.select_by_id('buttonConstructBuilding')

    def discard_card(self, name: str) -> None:
        self.select_by_name(name)
        self.select_by_id('buttonDiscardBuilding')

    def build_wonder(self, name: str, wonder_name: str) -> None:
        self.select_by_name(name)
        self.select_by_id('buttonConstructWonder')
        self.select_by_name(wonder_name)

    def choose_go_first(self) -> None:
        self.select_by_id('buttonPlayerLeft')

    def choose_go_second(self) -> None:
        self.select_by_id('buttonPlayerRight')

    def start_game(self) -> None:
        self.reset_state()
        self.open_bga_page()
        while True:
            try:
                self.create_game_btn()
            except Exception:
                pass
            try:
                self.open_table_btn()
            except Exception:
                pass
            try:
                self.start_game_btn()
            except Exception:
                pass
            try:
                self.accept_game_btn()
            except Exception:
                pass
            try:
                title_text = self.find_title_text()
                if title_text != '':
                    return
            except Exception:
                pass

    def reset_state(self) -> None:
        self.found_first_player = False
        self.found_start_player = True
        self.state = {}
        self.invalid_cnt = 0
        self.parser.reset_state()

    def assume_invalid(self) -> bool:
        return self.invalid_cnt >= BGAGame.MAX_INVALID_CNT

    def update_state(self, curr_state: dict[set]) -> int:

        def prev_elems(name: str) -> set:
            if name not in self.state:
                return set()
            return self.state[name]

        def curr_elems(name: str) -> set:
            if name not in curr_state:
                return set()
            return curr_state[name]

        def new_elems(name: str) -> set:
            return curr_elems(name).difference(prev_elems(name))

        def old_elems(name: str) -> set:
            if name not in self.state:
                return set()
            return prev_elems(name).difference(curr_elems(name))

        actions = []

        for game_token in new_elems('game_tokens'):
            actions.append(f'Reveal token, {game_token}')

        for box_token in new_elems('box_tokens'):
            actions.append(f'Reveal box token, {box_token}')

        for pos in new_elems('guild_poses'):
            actions.append(f'Reveal guild, {pos}')

        for pos, card in sorted(list(new_elems('pyramid_poses_cards'))):
            actions.append(f'Reveal card, {card}, {pos}')

        for wonder in new_elems('revealed_wonders'):
            actions.append(f'Reveal wonder, {wonder}')

        state_changed = len(actions) > 0

        if len(self.state) > 0:
            self.state['game_tokens'].update(new_elems('game_tokens'))
            self.state['box_tokens'].update(new_elems('box_tokens'))
            self.state['guild_poses'].update(new_elems('guild_poses'))
            self.state['pyramid_poses_cards'].update(new_elems('pyramid_poses_cards'))
            self.state['revealed_wonders'].update(new_elems('revealed_wonders'))

        for action in actions:
            self.pipe.write(action)

        actions = []

        gone_pyramid_cards = set(map(lambda pos_card: pos_card[1], old_elems('pyramid_poses_cards')))
        gone_built_cards = old_elems('built_cards')
        gone_discarded_cards = old_elems('discarded_cards')
        gone_game_tokens = old_elems('game_tokens')
        gone_box_tokens = old_elems('box_tokens')
        gone_revealed_wonders = old_elems('revealed_wonders')

        title_text = self.find_title_text()

        next_found_first_player = self.found_first_player

        if not self.found_first_player:
            revealed_wonders = curr_elems('revealed_wonders')
            selected_wonders = curr_elems('selected_wonders')
            curr_player = self.parse_curr_player(title_text)
            if curr_player is None or len(revealed_wonders) + len(selected_wonders) < 4:
                debug_print(f'Unknown first player')
                return BGAGame.STATE_INVALID
            if len(selected_wonders) == 0:
                actions.append(f'Reveal first player, {curr_player}')
            elif len(selected_wonders) == 1:
                actions.append(f'Reveal first player, {BGAGame.OTHER_PLAYER[curr_player]}')
            else:
                debug_print(f'Unexpected starting number of selected wonders, {selected_wonders}')
                return BGAGame.STATE_INVALID
            next_found_first_player = True

        next_found_start_player = self.found_start_player

        if 'must choose who begins' in title_text:
            next_found_start_player = False
        elif not self.found_start_player:
            curr_player = self.parse_curr_player(title_text)
            if curr_player is None:
                debug_print(f'Unknown start player')
                return BGAGame.STATE_INVALID
            else:
                actions.append(f'Choose start player, {curr_player}')
            next_found_start_player = True

        for card in new_elems('discarded_cards'):
            if card in gone_pyramid_cards:
                actions.append(f'Discard card, {card}')
                gone_pyramid_cards.remove(card)
            elif card in gone_built_cards:
                card_type = self.parser.card_types[self.parser.name_objects[card]]
                actions.append(f'Destroy {card_type}, {card}')
                gone_built_cards.remove(card)
            elif self.assume_invalid():
                for pos in range(BGAGame.PYRAMID_SIZE):
                    actions.append(f'Reveal card, {card}, {pos}')
                actions.append(f'Discard card, {card}')
            else:
                debug_print(f'Unexplained discarded card, {card}')
                return BGAGame.STATE_INVALID

        for card in new_elems('built_cards'):
            if card in gone_pyramid_cards:
                actions.append(f'Build card, {card}')
                gone_pyramid_cards.remove(card)
            elif card in gone_discarded_cards:
                actions.append(f'Build discarded, {card}')
                gone_discarded_cards.remove(card)
            elif self.assume_invalid():
                for pos in range(BGAGame.PYRAMID_SIZE):
                    actions.append(f'Reveal card, {card}, {pos}')
                actions.append(f'Build card, {card}')
            else:
                debug_print(f'Unexplained built card, {card}')
                return BGAGame.STATE_INVALID

        for wonder in new_elems('built_wonders'):
            if len(gone_pyramid_cards) > 0:
                card = gone_pyramid_cards.pop()
                actions.append(f'Build wonder, {card}, {wonder}')
            else:
                debug_print(f'Unexplained built wonder, {wonder}')
                return BGAGame.STATE_INVALID

        for token in new_elems('built_tokens'):
            if token in gone_game_tokens:
                actions.append(f'Build token, {token}')
                gone_game_tokens.remove(token)
            elif token in gone_box_tokens:
                actions.append(f'Build box token, {token}')
                gone_box_tokens.remove(token)
            elif len(prev_elems('box_tokens')) == 0 and len(curr_elems('box_tokens')) == 0:
                possible = sanitize_all(BGAGame.ALL_TOKENS)
                possible.difference_update(sanitize_all(curr_elems('game_tokens')))
                possible.difference_update(sanitize_all(curr_elems('built_tokens')))
                possible.discard(sanitize(token))
                actions.append(f'Reveal box token, {token}')
                if len(possible) > 0:
                    actions.append(f'Reveal box token, {possible.pop()}')
                if len(possible) > 0:
                    actions.append(f'Reveal box token, {possible.pop()}')
                actions.append(f'Build box token, {token}')
            else:
                debug_print(f'Unexplained built token, {token}')
                return BGAGame.STATE_INVALID

        for wonder in new_elems('selected_wonders'):
            if wonder in gone_revealed_wonders:
                actions.append(f'Select wonder, {wonder}')
                gone_revealed_wonders.remove(wonder)
            elif self.assume_invalid() and wonder not in prev_elems('revealed_wonders') and wonder not in curr_elems('revealed_wonders'):
                actions.append(f'Reveal wonder, {wonder}')
                actions.append(f'Select wonder, {wonder}')
            else:
                debug_print(f'Unexplained selected wonder, {wonder}')
                return BGAGame.STATE_INVALID

        if not self.assume_invalid():
            if len(gone_pyramid_cards) > 0:
                debug_print(f'Unexplained gone pyramid cards, {gone_pyramid_cards}')
                return BGAGame.STATE_INVALID

            if len(gone_built_cards) > 0:
                debug_print(f'Unexplained gone built cards, {gone_built_cards}')
                return BGAGame.STATE_INVALID

            if len(gone_discarded_cards) > 0:
                debug_print(f'Unexplained gone discarded cards, {gone_discarded_cards}')
                return BGAGame.STATE_INVALID

            if len(gone_game_tokens) > 0:
                debug_print(f'Unexplained gone tokens, {gone_game_tokens}')
                return BGAGame.STATE_INVALID

            if len(gone_revealed_wonders) > 0:
                debug_print(f'Unexplained gone revealed wonders, {gone_revealed_wonders}')
                return BGAGame.STATE_INVALID

        for action in actions:
            self.pipe.write(action)

        state_changed = state_changed or \
            curr_state != self.state or \
            next_found_first_player != self.found_first_player or \
            next_found_start_player != self.found_start_player

        self.state = curr_state
        self.found_first_player = next_found_first_player
        self.found_start_player = next_found_start_player

        if state_changed:
            return BGAGame.STATE_CHANGED
        else:
            return BGAGame.STATE_UNCHANGED

    def read_message(self) -> str:
        message = self.pipe.read()
        return sanitize(message)

    def wait_to_start(self) -> None:
        while True:
            if self.read_message() == sanitize('Start game'):
                return

    def process_action(self, action: str) -> None:
        act_type, *args = list(map(sanitize, action.split(',')))

        if act_type == sanitize('choose start player'):
            self.found_start_player = False

        while True:
            debug_print(f'Action: {act_type} {args}')

            try:
                if act_type == sanitize('build card'):
                    debug_print(f'Build card')
                    self.build_card(args[0])
                elif act_type == sanitize('discard card'):
                    debug_print(f'Discard card')
                    self.discard_card(args[0])
                elif act_type == sanitize('build wonder'):
                    debug_print(f'Build wonder')
                    self.build_wonder(args[0], args[1])
                elif act_type == sanitize('build token'):
                    debug_print(f'Build token')
                    self.select_by_name(args[0])
                elif act_type == sanitize('build box token'):
                    debug_print(f'Build box token')
                    self.select_by_name(args[0])
                elif act_type == sanitize('build discarded'):
                    debug_print(f'Build discarded')
                    self.select_by_name(args[0])
                elif sanitize('destroy') in act_type:
                    debug_print(f'Destroy card')
                    self.select_by_name(args[0])
                elif act_type == sanitize('select wonder'):
                    if args[0] not in self.state['revealed_wonders']:
                        debug_print(f'Skipped')
                        break
                    debug_print(f'Select wonder')
                    self.select_by_name(args[0])
                elif act_type == sanitize('choose start player'):
                    debug_print(f'Choose start player')
                    if args[0] == sanitize(BGAGame.PLAYER_ME):
                        self.choose_go_first()
                    elif args[0] == sanitize(BGAGame.PLAYER_OTHER):
                        self.choose_go_second()
                    else:
                        debug_print(f'Unknown player: {args[0]}')
                        break
                else:
                    debug_print(f'Unknown action type: {act_type}')
                    break
            except Exception as e:
                debug_print(e)
                pass

            try:
                res = self.check_update_state()
                debug_print(f'check_update_state res: {res}')
                if act_type == sanitize('choose start player') and not self.found_start_player:
                    continue
                if res != BGAGame.STATE_UNCHANGED:
                    debug_print(f'Action done')
                    break
            except Exception as e:
                debug_print(e)
                pass

    def check_update_state(self) -> int:
        old_state = self.state
        self.parser.start_parse()
        self.parser.feed(self.driver.page_source)
        curr_state = self.parser.get_state()
        res = self.update_state(curr_state)
        if res == BGAGame.STATE_INVALID:
            debug_print(f'Old state: {old_state}')
            debug_print(f'Invalid state: {curr_state}')
            self.invalid_cnt += 1
        elif res == BGAGame.STATE_CHANGED:
            debug_print(f'Old state: {old_state}')
            debug_print(f'New state: {curr_state}')
            self.invalid_cnt = 0
        return res

    def play_game(self) -> None:
        while True:
            try:
                res = self.check_update_state()
                if res == BGAGame.STATE_INVALID:
                    continue
                message = self.read_message()
                if message == sanitize('End game'):
                    return
                if message != '':
                    self.process_action(message)
            except Exception as e:
                debug_print(e)
                pass

def main() -> None:
    pipe = PipeReaderWriter('//./pipe/7wdai')
    game = BGAGame(pipe)

    while True:
        game.wait_to_start()
        game.start_game()
        game.play_game()
        time.sleep(5)

if __name__ == '__main__':
    main()
