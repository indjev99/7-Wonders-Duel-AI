from PIL import Image

def do_crop(path, N, M, names):
    assert(len(names) == N)

    for i in range(N):
        assert(len(names[i]) == M)

    img = Image.open(path)
    w = img.size[0] / M
    h = img.size[1] / N

    assert(w == int(w))
    assert(h == int(h))

    def to_img_name(name):
        name = name.replace(' ', '_')
        return f'imgs/{name}.png'

    for i in range(N):
        for j in range(M):
            name = names[i][j]
            if name == 'none':
                continue
            x = j * w
            y = i * h
            reg = (x, y, x + w, y + h)
            obj = img.crop(reg)
            obj.save(to_img_name(name))

def cards():
    path = 'orig/buildings.jpg'
    N = 8
    M = 12

    names = [
        ['lumber yard',
        'stone pit',
        'clay pool',
        'logging camp',
        'quarry',
        'clay pit',
        'glassworks',
        'press',
        'stable',
        'garrison',
        'palisade',
        'guard tower'],
        ['scriptorium',
        'workshop',
        'pharmacist',
        'apothecary',
        'tavern',
        'stone reserve',
        'clay reserve',
        'wood reserve',
        'theater',
        'altar',
        'baths',
        'sawmill'],
        ['shelf quarry',
        'brickyard',
        'glassblower',
        'drying room',
        'horse breeders',
        'barracks',
        'walls',
        'archery range',
        'parade ground',
        'school',
        'laboratory',
        'dispensary'],
        ['library',
        'brewery',
        'forum',
        'caravansery',
        'customs house',
        'temple',
        'statue',
        'tribunal',
        'aqueduct',
        'rostrum',
        'circus',
        'arsenal'],
        ['siege workshop',
        'fortifications',
        'courthouse',
        'academy',
        'university',
        'study',
        'observatory',
        'arena',
        'chamber of commerce',
        'port',
        'lighthouse',
        'armory'],
        ['palace',
        'gardens',
        'pantheon',
        'town hall',
        'senate',
        'obelisk',
        'merchants guild',
        'shipowners guild',
        'builders guild',
        'magistrates guild',
        'scientists guild',
        'moneylenders guild'],
        ['tacticians guild',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'deck age 1',
        'deck age 2'],
        ['deck age 3',
        'deck guilds',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none']
    ]

    do_crop(path, N, M, names)

def wonders():
    path = 'orig/wonders.jpg'
    N = 4
    M = 5

    names = [
        ['the pyramids',
        'the colossus',
        'the great lighthouse',
        'the temple of artemis',
        'the mausoleum'],
        ['the great library',
        'piraeus',
        'the hanging gardens',
        'the statue of zeus',
        'the sphinx'],
        ['the appian way',
        'circus maximus',
        'none',
        'none',
        'none'],
        ['none',
        'deck wonders',
        'none',
        'none',
        'none']
    ]

    do_crop(path, N, M, names)

def tokens():
    path = 'orig/progress_tokens.jpg'
    N = 4
    M = 4

    names = [
        ['agriculture',
        'architecture',
        'economy',
        'law'],
        ['masonry',
        'mathematics',
        'philosophy',
        'strategy'],
        ['theology',
        'urbanism',
        'none',
        'none'],
        ['none',
        'none',
        'none',
        'deck tokens']
    ]

    do_crop(path, N, M, names)

cards()
wonders()
tokens()
