from pipe_reader_writer import PipeReaderWriter
from paths import driver_path, browser_path, profile_path

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service

def sanitize(name: str) -> str:
    return ''.join(filter(lambda x: x.isalpha() or x == ',' or x == ' ', name.lower()))

class BGAGame:
    def __init__(self):
        options = Options()
        options.binary_location = browser_path
        options.add_argument(f'--profile {profile_path}')
        self.driver = webdriver.Firefox(service=Service(driver_path), options=options)
        self.reset_state()

    def reset_state(self) -> None:
        self.names_to_ids = {}
        self.building_ids_to_names = {}

    def register_name_id(self, name: str, id: str) -> None:
        self.names_to_ids[name] = id

    def find_cards(self) -> tuple[set[int], set[tuple[int, str]], set[str], set[str]]:
        pyramid_poses_cards = set()
        guild_poses = set()
        built_cards = set()
        discarded_cards = set()

        for card in self.driver.find_elements(By.CLASS_NAME, 'building'):
            card_name = sanitize(card.text)
            card_id = card.get_attribute('id')
            card_bulding_id = card.get_attribute('data-building-id')

            if card_bulding_id == '' or card_bulding_id is None:
                back_pos = card.value_of_css_property('background-position')
                if back_pos == '-100% -700%':
                    position = int(card.get_attribute('data-location'))
                    guild_poses.add(position)
                continue

            if card_name != '':
                self.building_ids_to_names[card_bulding_id] = card_name
            else:
                card_name = self.building_ids_to_names[card_bulding_id]

            self.register_name(card_name, card_id)

            container = card.find_element(By.XPATH, "../..")
            container_id = container.get_attribute('id')

            if container_id == 'draftpool_container':
                position = int(card.get_attribute('data-location'))
                pyramid_poses_cards.add((position, card_name))
                if 'GUILD' in card_name:
                    guild_poses.add(position)
            elif container_id == 'discarded_cards_container':
                discarded_cards.add(card_name)
            else:
                built_cards.add(card_name)

        return pyramid_poses_cards, guild_poses, built_cards, discarded_cards

    def find_tokens(self) -> tuple[set[str], set[str], set[str]]:
        game_tokens = set()
        box_tokens = set()
        built_tokens = set()

        for token in self.driver.find_elements(By.CLASS_NAME, 'progress_token'):
            token_name = sanitize(token.text)
            token_id = token.get_attribute('id')

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

    def find_wonders(self, can_have_built: bool) -> tuple[set[str], set[str], set[str]]:
        revealed_wonders = set()
        selected_wonders = set()
        built_wonders = set()

        for wonder in self.driver.find_elements(By.CLASS_NAME, 'wonder'):
            wonder_name = sanitize(wonder.text)
            wonder_id = wonder.get_attribute('id')

            self.register_name_id(wonder_name, wonder_id)

            constructed = int(wonder.get_attribute('data-constructed'))
            container = can_have_built and wonder.find_element(By.XPATH, "../../../..")

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
        WebDriverWait(self.driver, 120).until(EC.element_to_be_clickable((By.ID, id))).click()

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
        self.reset_state()
        self.open_bga_page()
        self.create_game()
        self.open_table()
        self.accept_game()

    def play_game(self) -> None:
        self.start_game()

        can_have_built = False

        old_everything = []

        while True:
            try:
                pyramid_poses_cards, guild_poses, built_cards, discarded_cards = self.find_cards()
                game_tokens, box_tokens, built_tokens = self.find_tokens()
                can_have_built |= len(pyramid_poses_cards) > 0
                revealed_wonders, selected_wonders, built_wonders = self.find_wonders(can_have_built)

                everything = [pyramid_poses_cards, guild_poses, built_cards, discarded_cards, game_tokens, box_tokens, built_tokens, revealed_wonders, selected_wonders, built_wonders]

                if everything != old_everything:
                    old_everything = everything
                    print(everything)
                    print()

            except Exception:
                pass


def main() -> None:
    # pipe = PipeReaderWriter('//./pipe/7wdai')

    # while True:
    #     print(pipe.read())

    game = BGAGame()
    game.play_game()

if __name__ == '__main__':
    main()
