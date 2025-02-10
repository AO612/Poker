#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

typedef enum Suits { Spades = 0, Hearts = 1, Clubs = 2, Diamonds = 3 } Suits;
typedef enum Ranks { Ace = 1, Jack = 11, Queen = 12, King = 13 } Ranks;

typedef struct Card
{
  int suit;
  int rank;
  struct Card *next;
} Card;

typedef struct Deck
{
    int cards;
    struct Card *card;
} Deck;

typedef struct Coordinates
{
    int x;
    int y;
} Coordinates;

void InsertCard(Deck *, int, int);
void RemoveCard(Deck *, int, int);
void InitialiseDeck(Deck *);
void PrintDeck(Deck *);
void PrintCard(Card *);
void PickCards(Deck *, Deck *, int, int);
void PickCard(Deck *, Deck *);
void DisplayHand(Deck *, int, int, Rectangle, int, Texture2D, Texture2D, Texture2D, Texture2D, int);
void DisplayCard(Card *, int, int, Rectangle, int, int, Texture2D, Texture2D, Texture2D, Texture2D, int);
Coordinates CardInformation(Deck*, int);
void TransferCardFromTo(Deck *, Deck*, int, int);
int CalculateScore(Deck *);

int main()
{
    srand(time(0));

    int windowedScreenWidth = 750;
    int windowedScreenHeight = 750;

    int screenWidth = windowedScreenWidth;
    int screenHeight = windowedScreenHeight;

    int cardWidth = 80;
    int cardHeight = cardWidth/5*7;


    Deck deck;
    deck.cards = 0;
    Deck hand;
    hand.cards = 0;
    Deck selected;
    selected.cards = 0;
    Deck discard;
    discard.cards = 0;

    int handsize = 8;
    int selectedsize = 5;
    int discards = 5;
    int plays = 3;

    InitialiseDeck(&deck);

    // Create window
    InitWindow(screenWidth, screenHeight, "Cards");

    SetTargetFPS(60);

    Image spadeImage = LoadImage("spade.png");     // Loaded in CPU memory (RAM)
    Texture2D spadeTexture = LoadTextureFromImage(spadeImage);          // Image converted to texture, GPU memory (VRAM)
    UnloadImage(spadeImage);
    Image heartImage = LoadImage("heart.png");     // Loaded in CPU memory (RAM)
    Texture2D heartTexture = LoadTextureFromImage(heartImage);          // Image converted to texture, GPU memory (VRAM)
    UnloadImage(heartImage);
    Image clubImage = LoadImage("club.png");     // Loaded in CPU memory (RAM)
    Texture2D clubTexture = LoadTextureFromImage(clubImage);          // Image converted to texture, GPU memory (VRAM)
    UnloadImage(clubImage);
    Image diamondImage = LoadImage("diamond.png");     // Loaded in CPU memory (RAM)
    Texture2D diamondTexture = LoadTextureFromImage(diamondImage);          // Image converted to texture, GPU memory (VRAM)
    UnloadImage(diamondImage);

    int font = 24;
    int mouseAcross = 0;
    int mouseUp = 0;
    int deckAcross = screenWidth*0.75;
    int deckUp = screenHeight*0.2;
    int selectedBool;

    Image screenshotImage;

    char Hands[11][16] = {"High Card","Pair","Two Pair","Three of a Kind","Straight","Flush","Full House","Four of a Kind", "Straight Flush","Royal Flush",""};
    int winningHand = 10; // Empty


    while(!WindowShouldClose())
    {

        screenshotImage = LoadImageFromScreen();

        BeginDrawing();
        ClearBackground(DARKGREEN);

        Rectangle deckParameters = {deckAcross, deckUp, cardWidth, cardHeight};

        DrawText(TextFormat("%d, %d", mouseAcross, mouseUp), screenWidth*0.5, screenHeight*0.5, font, BLACK);

        DrawRectangle(deckParameters.x, deckParameters.y, deckParameters.width, deckParameters.height, MAROON);
        DrawText(TextFormat("Click\nto\ndraw\n%d/52", deck.cards), deckAcross + cardWidth*0.05, deckUp + cardHeight*0.05, font, BLACK);


        int handAcross = -cardWidth/2;
        int handUp = screenHeight*0.7;
        
        Rectangle handParameters = {handAcross, handUp, cardWidth, cardHeight};
        Rectangle cardParameters;
        int selectedhandUp = handUp - cardHeight - 10;
        Rectangle selectedParameters = {handAcross, selectedhandUp, cardWidth, cardHeight};

        DisplayHand(&hand, screenWidth, screenHeight, handParameters, font, spadeTexture, heartTexture, clubTexture, diamondTexture, selectedBool = 0);
        DisplayHand(&selected, screenWidth, screenHeight, selectedParameters, font, spadeTexture, heartTexture, clubTexture, diamondTexture, selectedBool = 1);
        
        DrawText(TextFormat("%d/8", hand.cards), screenWidth*0.78, screenHeight*0.9, font, BLACK);
        DrawText(TextFormat("%d/5", selected.cards), screenWidth*0.78, selectedhandUp, font, BLACK);

        Vector2 mousePosition = GetMousePosition();
        mouseAcross = mousePosition.x;
        mouseUp = mousePosition.y;

        Color colour = GetImageColor(screenshotImage, mouseAcross*2, mouseUp*2);
        DrawText(TextFormat("%d, %d, %d", colour.r, colour.g, colour.b), screenWidth*0.5, screenHeight*0.1, font, WHITE);

        Vector2 playButton = {screenWidth*0.32 , screenHeight*0.88};
        DrawRectangle(playButton.x, playButton.y, deckParameters.height, deckParameters.width, BLUE);
        DrawText(TextFormat("Play\n%d/3", plays), playButton.x + deckParameters.height*0.25, playButton.y + deckParameters.width*0.25, font, BLACK);

        Vector2 discardButton = {screenWidth*0.52,screenHeight*0.88};
        DrawRectangle(discardButton.x, discardButton.y, deckParameters.height, deckParameters.width, RED);
        DrawText(TextFormat("Discard\n%d/5", discards), discardButton.x + deckParameters.height*0.15, discardButton.y + deckParameters.width*0.25, font, BLACK);
    

        if ( ColorIsEqual(colour, MAROON) )
        {
            DrawRectangleLinesEx(deckParameters, 1, WHITE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PickCards(&deck, &hand, selected.cards, handsize);
            }
        }

        Coordinates cardInformation;
        Color cardColour;
        Rectangle borderParameters;
        int loops = 0;

        for (int i = 0; i < hand.cards; i++)
        {
            cardColour = (Color){200, 200, 200+i, 255};
            if ( ColorIsEqual(colour, cardColour) )
            {
                borderParameters = handParameters;
                borderParameters.x += screenWidth/9*(i+1);
                borderParameters.x -= 1;
                borderParameters.y -= 1;
                borderParameters.width += 2;
                borderParameters.height += 2;
                DrawRectangleLinesEx(borderParameters, 2, WHITE);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    if ( selected.cards < selectedsize)
                    {
                        printf("%d\n",i);
                        cardInformation = CardInformation(&hand, i);
                        printf("%d %d\n", cardInformation.x, cardInformation.y);
                        TransferCardFromTo(&hand, &selected, cardInformation.x, cardInformation.y );
                    }
                }
            }
        
        }

        for (int i = 0; i < selected.cards; i++)
        {
            cardColour = (Color){201, 200, 200+i, 255};
            if ( ColorIsEqual(colour, cardColour) )
            {
                borderParameters = selectedParameters;
                borderParameters.x += screenWidth/9*(i+1);
                borderParameters.x -= 1;
                borderParameters.y -= 1;
                borderParameters.width += 2;
                borderParameters.height += 2;
                DrawRectangleLinesEx(borderParameters, 2, WHITE);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {

                    cardInformation = CardInformation(&selected, i);
                    TransferCardFromTo(&selected, &hand, cardInformation.x, cardInformation.y );
                }
            }
        
        }

        if ( ColorIsEqual(colour, RED) )
        {
            borderParameters = (Rectangle){discardButton.x, discardButton.y, cardHeight, cardWidth};
            borderParameters.x -= 1;
            borderParameters.y -= 1;
            borderParameters.width += 2;
            borderParameters.height += 2;
            DrawRectangleLinesEx(borderParameters, 2, WHITE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (selected.cards > 0)
                {
                    if ( discards > 0)
                    {
                        printf("selected cards: %d\n",selected.cards);
                        loops = selected.cards;
                        for (int i = 0; i < loops; i++)
                        {
                            cardInformation = CardInformation(&selected, 0);
                            TransferCardFromTo(&selected, &discard, cardInformation.x, cardInformation.y );
                        }
                        discards--;
                    }
                }    
            }
        }

        if ( ColorIsEqual(colour, BLUE) )
        {
            borderParameters = (Rectangle){playButton.x, playButton.y, cardHeight, cardWidth};
            borderParameters.x -= 1;
            borderParameters.y -= 1;
            borderParameters.width += 2;
            borderParameters.height += 2;
            DrawRectangleLinesEx(borderParameters, 2, WHITE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (selected.cards > 0)
                {
                    if (plays > 0)
                    {
                        winningHand = CalculateScore(&selected);
                        plays--;
                        loops = selected.cards;
                        for (int i = 0; i < loops; i++)
                        {
                            cardInformation = CardInformation(&selected, 0);
                            TransferCardFromTo(&selected, &deck, cardInformation.x, cardInformation.y );
                        }
                    }
                }
            }
        }

        DrawText(Hands[winningHand], screenWidth*0.1, screenHeight*0.1, font, BLACK);

        UnloadImage(screenshotImage);

        EndDrawing();

        if ( IsKeyPressed(KEY_R))
        {
            loops = hand.cards;
            for (int i = 0; i < loops; i++)
            {
                cardInformation = CardInformation(&hand, 0);
                TransferCardFromTo(&hand, &deck, cardInformation.x, cardInformation.y );
            }
            
            loops = selected.cards;
            for (int i = 0; i < loops; i++)
            {
                cardInformation = CardInformation(&selected, 0);
                TransferCardFromTo(&selected, &deck, cardInformation.x, cardInformation.y );
            }

            loops = discard.cards;
            for (int i = 0; i < loops; i++)
            {
                cardInformation = CardInformation(&discard, 0);
                TransferCardFromTo(&discard, &deck, cardInformation.x, cardInformation.y );
            }
            discards = 5;
            plays = 3;
        }


        if ( IsKeyPressed(KEY_F))
        {

            int display = GetCurrentMonitor();
            
            if (IsWindowFullscreen())
            {
                // if we are full screen, then go back to the windowed size
                screenWidth = windowedScreenWidth;
                screenHeight = windowedScreenHeight;
                SetWindowSize(screenWidth, screenHeight);
            }
            else
            {
                // if we are not full screen, set the window size to match the monitor we are on
                screenWidth = GetMonitorWidth(display);
                screenHeight = GetMonitorHeight(display);
                SetWindowSize(screenWidth, screenHeight);
            }

            deckAcross = screenWidth*0.75;
            deckUp = screenHeight*0.2;
    
            // toggle the state
            ToggleFullscreen();
        }

    }
    
    UnloadTexture(spadeTexture);
    UnloadTexture(heartTexture);
    UnloadTexture(clubTexture);
    UnloadTexture(diamondTexture);

    CloseWindow();

    return 0;
}

void InitialiseDeck(Deck *deck)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            InsertCard(deck, i, j+1);
        }
    }
}

void InsertCard(Deck *deck, int suit, int rank)
{
    if(deck->cards == 0) // First card in deck
    {
        deck->card = (struct Card*) malloc(sizeof(struct Card));
        deck->card->suit = suit;
        deck->card->rank = rank;
        deck->card->next = NULL;
    }

    else
    {
        struct Card *temp;
        temp = (struct Card*) malloc(sizeof(struct Card));
        temp->suit = suit;
        temp->rank = rank;
        if(( temp->suit*13 + temp->rank - 1 ) < ( deck->card->suit*13 + deck->card->rank - 1 ))
        {
            temp->next = deck->card;
            deck->card = temp;
        } // Insert from the front

        else if(deck->card->next == NULL)
        {
            deck->card->next = temp;
            temp->next = NULL;
        } // Insert at the end if there is only one card ahead
 
        else
        {
            struct Card *temp2;
            struct Card *temp3;
            temp2 = deck->card;
            temp3 = deck->card->next;
            while( temp3->next != NULL && ( temp->suit*13 + temp->rank - 1 ) > ( temp3->suit*13 + temp3->rank - 1 ))
            {
                temp2 = temp2->next;
                temp3 = temp3->next;
            }


            if(( temp->suit*13 + temp->rank - 1 ) < ( temp3->suit*13 + temp3->rank - 1 ))
            {
                temp2->next = temp;
                temp->next  = temp3;
            } // Add inbetween cards


            else if(temp3->next == NULL)
            {
                temp3->next = temp;
                temp->next = NULL;
            } // Add at the end
        }
    }
    deck->cards ++;
}

void PickCards(Deck *deck, Deck *hand, int selectedCards, int cards)
{
    printf("hand size   %d\n\n\n\n", hand->cards);
    printf("cards - hand->cards   %d\n\n\n\n", cards - hand->cards);
    int iterations = cards - hand->cards - selectedCards;

    for (int i = 0; i < iterations; i++)
    {
        printf(" i = %d\n\n\n\n", i);
        if (deck->cards != 0)
        {
            PickCard(deck, hand);
        }
        else
        {
            printf("No cards left\n");
            printf("BREAK\n");
            break;
        }
    }
}

void PickCard(Deck *deck, Deck *hand)
{
    int random = rand() % deck->cards;
    struct Card *temp;
    temp = deck->card;
    for (int i = 0; i < random; i++)
    {
        temp = temp->next;
    }
    InsertCard(hand, temp->suit, temp->rank);
    RemoveCard(deck, temp->suit, temp->rank);
}

void RemoveCard(Deck *deck, int suit, int rank)
{
    if (deck->cards != 0)
    {
        struct Card *temp;
        temp = deck->card;
        if ( ( suit*13 + rank - 1 ) == ( temp->suit*13 + temp->rank - 1 ) )
        {
            if(temp->next != NULL)
            {
                deck->card = temp->next;
                
                free(temp);
            }
            else
            {
                deck->card = NULL;
                free(temp);
            }
        }
        else // If the node that needs to be removed is not first in the list:
        {
            struct Card *temp2;
            temp2 = temp->next;
            if(( suit*13 + rank - 1 ) == ( temp2->suit*13 + temp2->rank - 1 ) && temp2->next == NULL) //This is used when there are only two nodes in the list and the second node needs to be removed.
            {
                temp->next = NULL;
                free(temp2);
            }
            else // If the linked list has three or more nodes;
            {
                while(( suit*13 + rank - 1 ) != ( temp2->suit*13 + temp2->rank - 1 ) )
                {
                    temp = temp->next;
                    temp2 = temp2->next;
                }
                //Iterate along the list until the two temp pointers are on either side of the node that needs to be removed. That node is then removed.
                temp->next = temp2->next;
                free(temp2);
            }
        }
        deck->cards--;
    }
    else
    {
        printf("This deck is empty\n");
    }
}

Coordinates CardInformation(Deck* deck, int position)
{
    if (deck->cards != 0)
    {
        struct Card *temp;
        temp = deck->card;
        for (int i = 0; i < position; i++)
        {
            temp = temp->next;
        }
        return (Coordinates){temp->suit, temp->rank};
    }
    else
    {
        printf("No cards in this deck\n");
        return (Coordinates){0, 0};
    }

}
void TransferCardFromTo(Deck * hand, Deck* selected, int suit, int rank)
{
    struct Card *temp;
    temp = hand->card;
    while( (suit*13 + rank - 1) != (temp->suit*13 + temp->rank - 1))
    {
        temp = temp->next;
    }
    InsertCard(selected, temp->suit, temp->rank);
    RemoveCard(hand, temp->suit, temp->rank);
}

int CalculateScore(Deck *hand)
{
    
    struct Card *temp;
    temp = hand->card;
    int sameSuit = 1;
    int array[14];
    for (int i = 0; i < 14; i++)
    {
        array[i] = 0;
    }
    array[(temp->rank)-1]++;
    if (temp->rank == 1)
    {
        array[13]++;
    }

    while (temp->next != NULL)
    {
        array[(temp->next->rank)-1]++;
        printf("current and next suit: %d %d\n", temp->suit , temp->next->suit);
        if (temp->next->suit != temp->suit)
        {
            sameSuit = 0;
            printf("notsamesuit\n");
        }
        temp = temp->next;
    }

    for (int i = 0; i < 14; i++)
    {
        printf("%d = %d   \n",i, array[i]);
    }

    if (hand->cards == 5)
    {
        
        if (sameSuit == 1)
        {
            for (int i = 9; i >= 0; i--)
            {
                if (array[i] == 1 && array[i+1] == 1 && array[i+2] == 1 && array[i+3] == 1 && array[i+4] == 1)
                {
                    if (i == 9)
                    {
                        return 9; // Royal Flush
                    }
                    else
                    {
                        return 8; // Straight flush
                    }
                }
            }
        }
    }
    

    for (int i = 0; i < 13; i++)
    {
        if (array[i] == 4)
        {
            return 7; // Four of a kind
        }
    }
    for (int i = 0; i < 13; i++)
    {
        if (array[i] == 3)
        {
            for (int j = 0; j < 13; j++)
            {
                if (i != j)
                {
                    if (array[j] == 2)
                    {
                        return 6; // Full house
                    }
                }
            }
        }
    }

    if (hand->cards == 5)
    {
        if (sameSuit == 1)
        {
            return 5; // Flush
        }
        for (int i = 9; i >= 0; i--)
        {
            if (array[i] == 1 && array[i+1] == 1 && array[i+2] == 1 && array[i+3] == 1 && array[i+4] == 1)
            {
                return 4; // Straight flush
            }
        }
    }
    for (int i = 0; i < 13; i++)
    {
        if (array[i] == 3)
        {
            return 3; // Three of a kind
        }
    }
    for (int i = 0; i < 13; i++)
    {
        if (array[i] == 2)
        {
            for (int j = 0; j < 13; j++)
            {
                if (i != j)
                {
                    if (array[j] == 2)
                    {
                        return 2; // Two pair
                    }
                }
            }
            return 1; // Pair
        }
    }

    return 0; // High card
    
}

void PrintDeck(Deck *deck)
{
    if (deck->cards != 0)
    {
        struct Card *temp;
        temp = deck->card;
        while (temp->next != NULL)
        {
            PrintCard(temp);
            temp = temp->next;
        }
        PrintCard(temp);
    }  
    else
    {
        printf("No cards\n");
    }

}

void PrintCard(Card *card)
{
    printf("Suit = ");
    if (card->suit == 0)
    {
        printf("Spades");
    }
    else if (card->suit == 1)
    {
        printf("Hearts");
    }
    else if (card->suit == 2)
    {
        printf("Clubs");
    }
    else if (card->suit == 3)
    {
        printf("Diamonds");
    }
    else
    {
        printf("%d", card->suit);
    }
    printf(" ");

    printf("Rank = ");
    if (card->rank == 1)
    {
        printf("Ace");
    }
    else if (card->rank == 11)
    {
        printf("Jack");
    }
    else if (card->rank == 12) 
    {
        printf("Queen");
    }
    else if (card->rank == 13)
    {
        printf("King");
    }
    else
    {
        printf("%d", card->rank);
    }
    printf("\n");
}

void DisplayHand(Deck *deck, int screenWidth, int screenHeight, Rectangle handParameters, int font, Texture2D spade, Texture2D heart, Texture2D club, Texture2D diamond, int selected)
{
    if (deck->cards != 0)
    {
        struct Card *temp;
        temp = deck->card;
        for (int i = 0; i < deck->cards; i++)
        {
            DisplayCard(temp, screenWidth, screenHeight, handParameters, font, i, spade, heart, club, diamond, selected);
            temp = temp->next;
        }
    }  
}

void DisplayCard(Card *card, int screenWidth, int screenHeight, Rectangle handParameters, int font, int iteration, Texture2D spade, Texture2D heart, Texture2D club, Texture2D diamond, int selected)
{
    int x = handParameters.x + screenWidth/9*(iteration+1);
    int y = handParameters.y;
    float scale = 0.03;
    Color colour = { 200 + selected, 200, 200+iteration, 255 };
    DrawRectangle(x, y, handParameters.width, handParameters.height,  colour);
    Vector2 textureposition = {x, y};
    
    if (card->suit == 0)
    {
        DrawTextureEx(spade, textureposition ,0,scale, WHITE);
    }
    else if (card->suit == 1)
    {
        DrawTextureEx(heart, textureposition ,0,scale, WHITE);
    }
    else if (card->suit == 2)
    {
        DrawTextureEx(club, textureposition ,0,scale, WHITE);
    }
    else if (card->suit == 3)
    {
        DrawTextureEx(diamond, textureposition ,0,scale, WHITE);
    }
    else
    {
        printf("%d", card->suit);
    }

    if (card->rank == 1)
    {
        DrawText("A", x + handParameters.width*0.75, y, font, BLACK);
    }
    else if (card->rank == 11)
    {
        DrawText("J", x + handParameters.width*0.75, y, font, BLACK);
    }
    else if (card->rank == 12) 
    {
        DrawText("Q", x + handParameters.width*0.75, y, font, BLACK);
    }
    else if (card->rank == 13)
    {
        DrawText("K", x + handParameters.width*0.75, y, font, BLACK);
    }
    else
    {
        DrawText(TextFormat("%d", card->rank), x+ handParameters.width*0.75, y, font, BLACK);
    }
}
	