from pipe_reader_writer import PipeReaderWriter
from paths import driver_path, browser_path, profile_path

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service

import sys

def sanitize(name: str) -> str:
    return ''.join(filter(lambda x: x.isalpha() or x == ',' or x == ' ', name.lower()))

class BGAGame:

    NONE = -100

    PLAYER_ME = 'Player 1'
    PLAYER_OTHER = 'Player 2'

    OTHER_PLAYER = {
        'Player 1' : 'Player 2',
        'Player 2' : 'Player 1'
    }

    def __init__(self, pipe : PipeReaderWriter):
        self.pipe = pipe

        options = Options()
        options.binary_location = browser_path
        options.add_argument(f'--profile {profile_path}')
        self.driver = webdriver.Firefox(service=Service(driver_path), options=options)
        self.reset_state()

    def register_name_id(self, name: str, id: str) -> None:
        self.names_to_ids[name] = id

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

    def find_cards(self) -> tuple[set[int], set[tuple[int, str]], set[str], set[str]]:
        pyramid_poses_cards = set()
        guild_poses = set()
        built_cards = set()
        discarded_cards = set()

        new_total_pyramid_cards = 0

        for card in self.driver.find_elements(By.CLASS_NAME, 'building'):
            card_name = sanitize(card.text)
            card_id = card.get_attribute('id')
            card_bulding_id = card.get_attribute('data-building-id')

            container = card.find_element(By.XPATH, "../..")
            container_id = container.get_attribute('id')

            card_loc_str = card.get_attribute('data-location')

            if container_id == 'draftpool_container':
                new_total_pyramid_cards += 1

                back_pos = card.value_of_css_property('background-position')
                if back_pos == '-100% -700%':
                    position = int(card_loc_str)
                    guild_poses.add(position)

            if card_bulding_id == '' or card_bulding_id is None:
                continue

            if card_name != '':
                self.building_ids_to_names[card_bulding_id] = card_name
            else:
                card_name = self.building_ids_to_names[card_bulding_id]

            if card_name == '' or card_name is None:
                continue

            self.register_name_id(card_name, card_id)

            if container_id == 'draftpool_container':
                position = int(card_loc_str)
                pyramid_poses_cards.add((position, card_name))
                if 'GUILD' in card_name:
                    guild_poses.add(position)
            elif container_id == 'discarded_cards_container':
                discarded_cards.add(card_name)
            else:
                built_cards.add(card_name)

        if new_total_pyramid_cards > self.total_pyramid_cards:
            pyramid_poses = set(map(lambda pos_card : pos_card[0], pyramid_poses_cards))
            if 0 not in pyramid_poses:
                raise Exception()

        self.total_pyramid_cards = new_total_pyramid_cards

        if self.total_pyramid_cards > 0:
            self.wonder_selection = False

        return pyramid_poses_cards, guild_poses, built_cards, discarded_cards

    def find_tokens(self) -> tuple[set[str], set[str], set[str]]:
        game_tokens = set()
        box_tokens = set()
        built_tokens = set()

        for token in self.driver.find_elements(By.CLASS_NAME, 'progress_token'):
            token_name = sanitize(token.text)
            token_id = token.get_attribute('id')

            if token_id == '' or token_id is None:
                continue

            if token_name == '' or token_name is None:
                continue

            self.register_name_id(token_name, token_id)

            container = token.find_element(By.XPATH, "../..")
            container_id = container.get_attribute('id')

            if container_id == 'board_progress_tokens':
                game_tokens.add(token_name)
            elif container_id == 'progress_token_from_box_container':
                box_tokens.add(token_name)
            else:
                built_tokens.add(token_name)

        return game_tokens, box_tokens, built_tokens

    def find_wonders(self) -> tuple[set[str], set[str], set[str]]:
        revealed_wonders = set()
        selected_wonders = set()
        built_wonders = set()

        for wonder in self.driver.find_elements(By.CLASS_NAME, 'wonder'):
            wonder_name = sanitize(wonder.text)
            wonder_id = wonder.get_attribute('id')

            if wonder_id == '' or wonder_id is None:
                continue

            if wonder_name == '' or wonder_name is None:
                continue

            self.register_name_id(wonder_name, wonder_id)

            constructed = not self.wonder_selection and bool(int(wonder.get_attribute('data-constructed')))
            container = wonder.find_element(By.XPATH, "../../../..")

            if container.get_attribute('id') == 'wonder_selection_block':
                revealed_wonders.add(wonder_name)
            elif constructed:
                built_wonders.add(wonder_name)
            else:
                selected_wonders.add(wonder_name)

        if len(revealed_wonders) > 0:
            selected_wonders |= built_wonders
            built_wonders = set()

        return revealed_wonders, selected_wonders, built_wonders

    def open_bga_page(self) -> None:
        self.driver.get("https://boardgamearena.com/lobby")

    def select_by_id(self, id: str) -> None:
        while True:
            try:
                WebDriverWait(self.driver, 120).until(EC.element_to_be_clickable((By.ID, id))).click()
                return
            except Exception:
                pass

    def select_by_name(self, name: str) -> None:
        id = self.names_to_ids[name]
        self.select_by_id(id)

    def create_game(self) -> None:
        self.select_by_id('joingame_create_1266')

    def open_table(self) -> None:
        self.select_by_id('open_table_now')

    def accept_game(self) -> None:
        self.select_by_id('ags_start_game_accept')

    def discard_card(self, name: str) -> None:
        self.select_by_name(name)
        self.select_by_id('buttonDiscardBuilding')

    def build_card(self, name: str) -> None:
        self.select_by_name(name)
        self.select_by_id('buttonConstructBuilding')

    def build_wonder(self, name: str, wonder_name: str) -> None:
        self.select_by_name(name)
        self.select_by_id('buttonConstructWonder')
        self.select_by_name(wonder_name)

    def choose_go_first(self) -> None:
        self.select_by_id('buttonPlayerLeft')

    def choose_go_second(self) -> None:
        self.select_by_id('buttonPlayerRight')

    def start_game(self) -> None:
        self.open_bga_page()
        self.create_game()
        self.open_table()
        self.accept_game()

    def reset_state(self) -> None:
        self.names_to_ids = {}
        self.building_ids_to_names = {}
        self.wonder_selection = True
        self.curr_age = -1
        self.found_first_player = False
        self.total_pyramid_cards = 0
        self.found_start_player = True
        self.state = {}

    def update_state(self, curr_state: dict[set]) -> None:
        if curr_state == self.state and self.found_first_player and self.found_start_player:
            return

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

        new_pyramid_poses = set(map(lambda pos_card: pos_card[0], new_elems('pyramid_poses_cards')))
        gone_pyramid_cards = set(map(lambda pos_card: pos_card[1], old_elems('pyramid_poses_cards')))
        gone_built_cards = old_elems('built_cards')
        gone_discarded_cards = old_elems('discarded_cards')
        gone_game_tokens = old_elems('game_tokens')
        gone_box_tokens = old_elems('box_tokens')
        gone_revealed_wonders = old_elems('revealed_wonders')

        next_found_first_player = self.found_first_player

        if not self.found_first_player:
            revealed_wonders = curr_elems('revealed_wonders')
            title_text = self.find_title_text()
            curr_player = self.parse_curr_player(title_text)
            if curr_player is None:
                print(f'Unknown first player', file=sys.stderr)
                return
            if len(revealed_wonders) == 4:
                actions.append(f'Reveal first player, {curr_player}')
            elif len(revealed_wonders) == 3:
                actions.append(f'Reveal first player, {BGAGame.OTHER_PLAYER[curr_player]}')
            else:
                print(f'Unexpected starting number of revealed wonders, {revealed_wonders}', file=sys.stderr)
                return
            next_found_first_player = True

        next_age = self.curr_age

        next_found_start_player = self.found_start_player

        if 0 in new_pyramid_poses:
            next_found_start_player = False
            next_age += 1

        if not self.found_start_player or not next_found_start_player:
            title_text = self.find_title_text()
            if self.curr_age >= 1 or next_age >= 1 and 'must choose who begins' not in title_text:
                curr_player = self.parse_curr_player(title_text)
                if curr_player is None:
                    print(f'Unknown start player', file=sys.stderr)
                    return
                if self.total_pyramid_cards == 20:
                    actions.append(f'Choose start player, {curr_player}')
                elif self.total_pyramid_cards == 19:
                    actions.append(f'Choose start player, {BGAGame.OTHER_PLAYER[curr_player]}')
                else:
                    print(f'Unexpected starting number pyramid cards, {self.total_pyramid_cards}', file=sys.stderr)
                    return
                next_found_start_player = True

        for card in new_elems('discarded_cards'):
            if card in gone_pyramid_cards:
                actions.append(f'Discard card, {card}')
                gone_pyramid_cards.remove(card)
            elif card in gone_built_cards:
                # TODO: color
                actions.append(f'Destroy TODO, {card}')
                gone_built_cards.remove(card)
            else:
                print(f'Unexplained discarded card, {card}', file=sys.stderr)
                return

        for card in new_elems('built_cards'):
            if card in gone_pyramid_cards:
                actions.append(f'Build card, {card}')
                gone_pyramid_cards.remove(card)
            elif card in gone_discarded_cards:
                actions.append(f'Build discarded, {card}')
                gone_discarded_cards.remove(card)
            else:
                print(f'Unexplained built card, {card}', file=sys.stderr)
                return

        for wonder in new_elems('built_wonders'):
            if len(gone_pyramid_cards) > 0:
                card = gone_pyramid_cards.pop()
                actions.append(f'Build wonder, {card}, {wonder}')
            else:
                print(f'Unexplained built wonder, {wonder}', file=sys.stderr)
                return

        for token in new_elems('built_tokens'):
            if token in gone_game_tokens:
                actions.append(f'Build token, {token}')
                gone_game_tokens.remove(token)
            elif token in gone_box_tokens:
                actions.append(f'Build box token, {token}')
                gone_game_tokens.remove(token)
            elif len(prev_elems('box_tokens')) == 0 and len(curr_elems('box_tokens')) == 0:
                # TODO: Reveal 2 random box tokens
                actions.append(f'Reveal box token, {token}')
                actions.append(f'Build box token, {token}')
            else:
                print(f'Unexplained built token, {token}', file=sys.stderr)
                return

        for wonder in new_elems('selected_wonders'):
            if wonder in gone_revealed_wonders:
                actions.append(f'Select wonder, {wonder}')
                gone_revealed_wonders.remove(wonder)
            elif wonder not in prev_elems('revealed_wonders') and wonder not in curr_elems('revealed_wonders'):
                actions.append(f'Reveal wonder, {wonder}')
                actions.append(f'Select wonder, {wonder}')
            else:
                print(f'Unexplained selected wonder, {wonder}', file=sys.stderr)
                return

        if len(gone_pyramid_cards) > 0:
            print(f'Unexplained gone pyramid cards, {gone_pyramid_cards}', file=sys.stderr)
            return

        if len(gone_built_cards) > 0:
            print(f'Unexplained gone built cards, {gone_built_cards}', file=sys.stderr)
            return

        if len(gone_discarded_cards) > 0:
            print(f'Unexplained gone discarded cards, {gone_discarded_cards}', file=sys.stderr)
            return

        if len(gone_game_tokens) > 0:
            print(f'Unexplained gone tokens, {gone_game_tokens}', file=sys.stderr)
            return

        if len(gone_revealed_wonders) > 0:
            print(f'Unexplained gone revealed wonders, {gone_revealed_wonders}', file=sys.stderr)
            return

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

        for action in actions:
            self.pipe.write(action)
            print(action)

        self.state = curr_state

        self.found_first_player = next_found_first_player
        self.curr_age = next_age
        self.found_start_player = next_found_start_player

    def play_game(self) -> None:
        self.reset_state()

        while True:
            try:
                pyramid_poses_cards, guild_poses, built_cards, discarded_cards = self.find_cards()
                game_tokens, box_tokens, built_tokens = self.find_tokens()
                revealed_wonders, selected_wonders, built_wonders = self.find_wonders()

                curr_state = {
                    'pyramid_poses_cards' : pyramid_poses_cards,
                    'guild_poses' : guild_poses,
                    'built_cards' : built_cards,
                    'discarded_cards' : discarded_cards,
                    'game_tokens' : game_tokens,
                    'box_tokens' : box_tokens,
                    'built_tokens' : built_tokens,
                    'revealed_wonders' : revealed_wonders,
                    'selected_wonders' : selected_wonders,
                    'built_wonders' : built_wonders
                }

                self.update_state(curr_state)

            except Exception:
                pass


def main() -> None:
    pipe = PipeReaderWriter('//./pipe/7wdai')

    game = BGAGame(pipe)
    game.start_game()
    game.play_game()

if __name__ == '__main__':
    main()
